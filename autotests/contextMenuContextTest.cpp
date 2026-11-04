// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2026 zigai

#include "contextMenuContext.h"
#include "fileModel.h"
#include "fileTree.h"

#include <QClipboard>
#include <QGuiApplication>
#include <QMimeData>
#include <QSignalSpy>
#include <QTest>

class ContextMenuContextTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void cleanup()
    {
        Filelight::FileModel::instance()->setTree({});
    }

    void copyMultipleUrls()
    {
        const QList<QUrl> urls{QUrl::fromLocalFile(QStringLiteral("/tmp/one/")),
                               QUrl::fromLocalFile(QString::fromUtf8("/tmp/café #2.txt")),
                               QUrl(QStringLiteral("sftp://example.org/three"))};
        Filelight::ContextMenuContext context;
        context.copyUrls(urls);
        const auto *mime = QGuiApplication::clipboard()->mimeData();
        QCOMPARE(mime->urls(), urls);
        QCOMPARE(mime->text(), QString::fromUtf8("/tmp/one/\n/tmp/café #2.txt\nsftp://example.org/three"));
        context.copyUrls({});
        QCOMPARE(QGuiApplication::clipboard()->mimeData()->urls(), urls);
    }

    void selectedRows()
    {
        auto tree = std::make_shared<Folder>("/tmp/");
        tree->append("one.txt", 1);
        tree->append(std::make_shared<Folder>("two/"));
        auto *model = Filelight::FileModel::instance();
        model->setTree(tree);
        QVERIFY(!model->file(-1));
        QVERIFY(!model->file(2));
        const QList<QUrl> expected{tree->files.at(0)->url(), tree->files.at(1)->url()};
        QCOMPARE(model->urls({0, 1, 0, -1, 999}), expected);
        model->setTree({});
        QVERIFY(!model->file(0));
        QVERIFY(model->urls({0}).isEmpty());
    }

    void batchModelRemoval()
    {
        auto tree = std::make_shared<Folder>("/tmp/");
        tree->append("one.txt", 1);
        tree->append("two.txt", 1);
        tree->append("three.txt", 1);
        auto *model = Filelight::FileModel::instance();
        model->setTree(tree);
        const auto lastIndex = model->index(2, 0);
        bool sawIntactRows = false;
        const auto connection = connect(model, &QAbstractItemModel::modelAboutToBeReset, this, [&] {
            sawIntactRows = model->rowCount({}) == 3 && model->data(lastIndex, Qt::DisplayRole).isValid();
        });
        QSignalSpy resets(model, &QAbstractItemModel::modelReset);
        const auto survivor = model->file(1);
        model->removeFiles({model->file(0), model->file(2)});
        disconnect(connection);
        QVERIFY(sawIntactRows);
        QCOMPARE(resets.count(), 1);
        QCOMPARE(model->rowCount({}), 1);
        QCOMPARE(model->file(0), survivor);
        QVERIFY(!model->data(lastIndex, Qt::DisplayRole).isValid());
    }

    void emptyDeletion()
    {
        Filelight::ContextMenuContext context;
        QSignalSpy changes(&context, &Filelight::ContextMenuContext::deletingChanged);
        context.deleteRows({});
        context.deleteRows({-1, 999});
        QCOMPARE(changes.count(), 0);
    }

    void copyClipboard_data()
    {
        QTest::addColumn<QUrl>("url");
        QTest::addColumn<QString>("text");

        QTest::newRow("file") << QUrl::fromLocalFile(QStringLiteral("/tmp/file.txt")) << QStringLiteral("/tmp/file.txt");
        QTest::newRow("folder") << QUrl::fromLocalFile(QStringLiteral("/tmp/folder/")) << QStringLiteral("/tmp/folder/");
        QTest::newRow("special-characters") << QUrl::fromLocalFile(QString::fromUtf8("/tmp/café #1 100%.txt")) << QString::fromUtf8("/tmp/café #1 100%.txt");
        const QUrl remote(QStringLiteral("sftp://example.org/home/user/a%20file.txt"));
        QTest::newRow("remote") << remote << remote.toDisplayString();
    }

    void copyClipboard()
    {
        QFETCH(QUrl, url);
        QFETCH(QString, text);

        Filelight::ContextMenuContext context;
        context.copyClipboard(url);

        const auto *mime = QGuiApplication::clipboard()->mimeData();
        QVERIFY(mime);
        QVERIFY(mime->formats().contains(QStringLiteral("text/plain")));
        QVERIFY(mime->hasText());
        QCOMPARE(mime->text(), text);
        QVERIFY(mime->hasUrls());
        QCOMPARE(mime->urls(), QList<QUrl>{url});
    }
};

QTEST_MAIN(ContextMenuContextTest)

#include "contextMenuContextTest.moc"
