// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2022 Harald Sitter <sitter@kde.org>

#include "contextMenuContext.h"

#include <QClipboard>
#include <QGuiApplication>
#include <QMimeData>

#include <KIO/DeleteJob>
#include <KLocalizedString>
#include <KMessageBox>
#include <KTerminalLauncherJob>
#include <KUrlMimeData>

#include "Config.h"
#include "fileModel.h"
#include "radialMap/map.h"
#include "radialMap/radialMap.h"
#include "scan.h"

using namespace Qt::StringLiterals;

namespace Filelight
{

void ContextMenuContext::openTerminal(RadialMap::Segment *segment)
{
    openTerminal(segment->url());
}

void ContextMenuContext::openTerminal(const QUrl &url)
{
    Q_ASSERT(url.scheme() == "file"_L1); // terminals generally only support local paths
    auto *job = new KTerminalLauncherJob(QString(), this);
    job->setWorkingDirectory(url.toLocalFile());
    job->start();
}

void ContextMenuContext::doNotScan(RadialMap::Segment *segment)
{
    doNotScan(segment->url());
}

void ContextMenuContext::doNotScan(const QUrl &url)
{
    if (!Config::instance()->skipList.contains(url.toLocalFile())) {
        Config::instance()->skipList.append(url.toLocalFile());
        Config::instance()->write();
    }
}

void ContextMenuContext::copyClipboard(RadialMap::Segment *segment)
{
    copyClipboard(segment->url());
}

void ContextMenuContext::copyClipboard(const QUrl &url)
{
    copyUrls({url});
}

void ContextMenuContext::copyUrls(const QList<QUrl> &urls)
{
    if (urls.isEmpty()) {
        return;
    }
    QStringList paths;
    for (const auto &url : urls) {
        paths.append(url.isLocalFile() ? url.toLocalFile() : url.toDisplayString());
    }
    auto mime = new QMimeData;
    mime->setUrls(urls);
    mime->setText(paths.join(u'\n'));
    KUrlMimeData::exportUrlsToPortal(mime);
    QGuiApplication::clipboard()->setMimeData(mime, QClipboard::Clipboard);
}

static bool shouldDelete(const QList<std::shared_ptr<File>> &files)
{
    QString message;
    if (files.size() == 1) {
        const auto &file = files.first();
        const auto path = file->url().toDisplayString().toHtmlEscaped();
        message = file->isFolder() ? i18n("<qt>The folder at <i>'%1'</i> will be <b>recursively</b> and <b>permanently</b> deleted.</qt>", path)
                                   : i18n("<qt><i>'%1'</i> will be <b>permanently</b> deleted.</qt>", path);
    } else {
        QStringList names;
        for (const auto &file : files.first(qMin(files.size(), qsizetype(10)))) {
            names.append(QStringLiteral("<li>%1</li>").arg(file->url().toDisplayString().toHtmlEscaped()));
        }
        message = i18np("<qt>The selected item will be <b>permanently</b> deleted. Folders and their contents will be deleted recursively.<ul>%2</ul></qt>",
                        "<qt>All %1 selected items will be <b>permanently</b> deleted. Folders and their contents will be deleted recursively.<ul>%2</ul></qt>",
                        files.size(),
                        names.join(QString()));
        if (files.size() > 10) {
            message += i18np("And %1 more item.", "And %1 more items.", files.size() - 10);
        }
    }
    return KMessageBox::warningContinueCancel(nullptr, message, QString(), KGuiItem(i18n("&Delete"), QStringLiteral("edit-delete"))) == KMessageBox::Continue;
}

void ContextMenuContext::deleteRows(const QList<int> &rows)
{
    QList<std::shared_ptr<File>> files;
    for (const int row : rows) {
        if (const auto file = FileModel::instance()->file(row); file && !files.contains(file)) {
            files.append(file);
        }
    }
    deleteFiles(files);
}

void ContextMenuContext::deleteFileFromSegment(RadialMap::Segment *segment)
{
    deleteFile(segment->file());
}

void ContextMenuContext::deleteFile(const std::shared_ptr<File> &file)
{
    if (file) {
        deleteFiles({file});
    }
}

void ContextMenuContext::deleteFiles(const QList<std::shared_ptr<File>> &files)
{
    if (m_deleting || files.isEmpty()) {
        return;
    }

    // A displayed cached subtree has raw parent pointers. Retain its owning
    // cache roots too, and snapshot URLs before the modal dialog can run events.
    const auto tree = FileModel::instance()->m_tree;
    const auto roots = ScanManager::instance()->cachedTrees();
    QList<QUrl> urls;
    for (const auto &file : files) {
        urls.append(file->url());
    }
    setDeleting(true);
    if (!shouldDelete(files)) {
        setDeleting(false);
        return;
    }
    QGuiApplication::setOverrideCursor(Qt::BusyCursor);
    m_pendingDeletions = files.size();
    const auto removed = std::make_shared<QList<std::shared_ptr<File>>>();
    for (qsizetype i = 0; i < files.size(); ++i) {
        const auto file = files.at(i);
        auto *job = KIO::del(urls.at(i));
        connect(job, &KJob::finished, this, [this, job, file, tree, roots, removed] {
            if (!job->error()) {
                removed->append(file);
            } else {
                Q_EMIT deleteFileFailed(i18nc("@info:notification", "Error while deleting: %1", job->errorString()));
            }
            if (--m_pendingDeletions == 0) {
                // Do not change row storage between job completions: ListView
                // may still query those rows even while the page is disabled.
                FileModel::instance()->removeFiles(*removed);
                RadialMap::Map::instance()->refresh(Dirty::Layout);
                QGuiApplication::restoreOverrideCursor();
                setDeleting(false);
            }
        });
    }
}

void ContextMenuContext::setDeleting(bool status)
{
    m_deleting = status;
    Q_EMIT deletingChanged();
}

} // namespace Filelight
