// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2026 zigai

#include "navigationHistory.h"

namespace Filelight
{

QUrl NavigationHistory::normalized(const QUrl &url)
{
    if (url.isEmpty() || !url.isValid() || url.isRelative()) {
        return {};
    }
    auto result = url.adjusted(QUrl::NormalizePathSegments);
    auto path = result.path();
    if (!path.endsWith(u'/')) {
        path += u'/';
        result.setPath(path);
    }
    return result;
}

bool NavigationHistory::record(const QUrl &url)
{
    const auto entry = normalized(url);
    if (entry.isEmpty() || entry == current()) {
        return false;
    }
    m_entries.resize(m_index + 1);
    m_entries.append(entry);
    m_index = m_entries.size() - 1;
    return true;
}

bool NavigationHistory::prependParent(const QUrl &url)
{
    const auto entry = normalized(url);
    if (m_index != 0 || entry.isEmpty() || entry == current()) {
        return false;
    }
    // Back on an initially opened folder still means Up, but retains that
    // folder (and any existing forward chain) so Forward can return to it.
    m_entries.prepend(entry);
    return true;
}

bool NavigationHistory::canGoBack() const
{
    return m_index > 0;
}

bool NavigationHistory::canGoForward() const
{
    return m_index >= 0 && m_index + 1 < m_entries.size();
}

QUrl NavigationHistory::current() const
{
    return m_index >= 0 ? m_entries.at(m_index) : QUrl();
}

QUrl NavigationHistory::back() const
{
    return canGoBack() ? m_entries.at(m_index - 1) : QUrl();
}

QUrl NavigationHistory::forward() const
{
    return canGoForward() ? m_entries.at(m_index + 1) : QUrl();
}

bool NavigationHistory::goBack()
{
    if (!canGoBack()) {
        return false;
    }
    --m_index;
    return true;
}

bool NavigationHistory::goForward()
{
    if (!canGoForward()) {
        return false;
    }
    ++m_index;
    return true;
}

} // namespace Filelight
