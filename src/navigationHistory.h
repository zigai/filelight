// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2026 zigai

#pragma once

#include <QList>
#include <QUrl>

namespace Filelight
{

// Folder URLs only; the history never owns scan trees or sidebar row indexes.
class NavigationHistory
{
public:
    bool record(const QUrl &url);
    bool prependParent(const QUrl &url);
    bool goBack();
    bool goForward();
    bool canGoBack() const;
    bool canGoForward() const;
    QUrl current() const;
    QUrl back() const;
    QUrl forward() const;

private:
    static QUrl normalized(const QUrl &url);
    QList<QUrl> m_entries;
    qsizetype m_index = -1;
};

} // namespace Filelight
