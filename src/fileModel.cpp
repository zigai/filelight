// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2022 Harald Sitter <sitter@kde.org>

#include "fileModel.h"

#include <QMetaEnum>
#include <QUrl>

#include "fileTree.h"

namespace Filelight
{

void FileModel::setTree(const std::shared_ptr<Folder> &tree)
{
    // NOTE: even reset the model if the root hasn't changed! When deleting an item in the tree the root doesn't change
    // but the tree will be different.
    beginResetModel();
    m_tree = tree;
    Q_EMIT treeChanged();
    endResetModel();
}

void FileModel::removeFiles(const QList<std::shared_ptr<File>> &files)
{
    if (files.isEmpty()) {
        return;
    }
    beginResetModel();
    for (const auto &file : files) {
        if (file && file->parent()) {
            file->parent()->remove(file);
        }
    }
    Q_EMIT treeChanged();
    endResetModel();
}

QUrl FileModel::url() const
{
    if (m_tree) {
        return m_tree->url();
    }
    return {};
}

int FileModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    if (!m_tree) {
        return 0;
    }
    return m_tree->files.size();
}

QVariant FileModel::data(const QModelIndex &index, int intRole) const
{
    if (!index.isValid() || !m_tree || index.row() < 0 || index.row() >= static_cast<int>(m_tree->files.size())) {
        return {};
    }
    const auto row = index.row();
    const auto &file = m_tree->files.at(row);
    switch (intRole) {
    case Qt::DisplayRole:
        return file->displayName();
    }

    switch (static_cast<Role>(intRole)) {
    case Role::HumanReadableSize:
        return file->humanReadableSize();
    case Role::IsFolder:
        return file->isFolder();
    case Role::URL:
        return file->url();
    case Role::Segment:
        return file->segment();
    }

    return {};
}

QHash<int, QByteArray> FileModel::roleNames() const
{
    static QHash<int, QByteArray> roles;
    if (!roles.isEmpty()) {
        return roles;
    }

    roles = QAbstractListModel::roleNames();
    const QMetaEnum roleEnum = QMetaEnum::fromType<Role>();
    for (int i = 0; i < roleEnum.keyCount(); ++i) {
        const int value = roleEnum.value(i);
        Q_ASSERT(value != -1);
        roles[static_cast<int>(value)] = QByteArray("ROLE_") + roleEnum.valueToKey(value);
    }
    return roles;
}

std::shared_ptr<File> FileModel::file(int row) const
{
    if (!m_tree || row < 0 || row >= static_cast<int>(m_tree->files.size())) {
        return {};
    }
    return m_tree->files.at(row);
}

bool FileModel::isFolder(int row) const
{
    const auto entry = file(row);
    return entry && entry->isFolder();
}

QList<QUrl> FileModel::urls(const QList<int> &rows) const
{
    QList<QUrl> result;
    for (const int row : rows) {
        if (const auto entry = file(row); entry && !result.contains(entry->url())) {
            result.append(entry->url());
        }
    }
    return result;
}

FileModel *FileModel::instance()
{
    static FileModel model;
    return &model;
}

FileModel *FileModel::create(QQmlEngine *qml, QJSEngine *js)
{
    auto model = instance();
    QQmlEngine::setObjectOwnership(model, QQmlEngine::CppOwnership);
    return model;
}

FileModel::FileModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

} // namespace Filelight
