// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2026 zigai

#include "navigationInput.h"

#include <QKeyEvent>
#include <QKeySequence>
#include <QMouseEvent>

void NavigationInput::setWindow(QWindow *window)
{
    if (window == m_window) {
        return;
    }
    if (m_window) {
        m_window->removeEventFilter(this);
    }
    m_window = window;
    if (m_window) {
        m_window->installEventFilter(this);
    }
    Q_EMIT windowChanged();
}

QWindow *NavigationInput::window() const
{
    return m_window;
}

bool NavigationInput::eventFilter(QObject *watched, QEvent *event)
{
    if (!m_enabled || watched != m_window) {
        return false;
    }

    switch (event->type()) {
    case QEvent::ShortcutOverride:
    case QEvent::KeyPress:
    case QEvent::KeyRelease: {
        const auto *key = static_cast<QKeyEvent *>(event);
        if (key->key() != Qt::Key_Back && !key->matches(QKeySequence::Back)) {
            return false;
        }
        // Claim ShortcutOverride so the framework's Back shortcut never runs.
        event->accept();
        if (event->type() == QEvent::KeyPress && !key->isAutoRepeat()) {
            Q_EMIT backRequested();
        }
        return true;
    }
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick: {
        const auto *mouse = static_cast<QMouseEvent *>(event);
        if (mouse->button() != Qt::BackButton) {
            return false;
        }
        event->accept();
        // One action per click, never on both press and release.
        if (event->type() == QEvent::MouseButtonRelease) {
            Q_EMIT backRequested();
        }
        return true;
    }
    default:
        return false;
    }
}
