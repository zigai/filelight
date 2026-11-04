// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2026 zigai

#pragma once

#include <QObject>
#include <QPointer>
#include <QWindow>
#include <qqmlintegration.h>

// Handle folder navigation before Kirigami's generic page-stack shortcuts.
class NavigationInput : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QWindow *window READ window WRITE setWindow NOTIFY windowChanged REQUIRED)
    Q_PROPERTY(bool enabled MEMBER m_enabled)

public:
    using QObject::QObject;
    void setWindow(QWindow *window);
    QWindow *window() const;

Q_SIGNALS:
    void windowChanged();
    void backRequested();
    void forwardRequested();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QPointer<QWindow> m_window;
    bool m_enabled = false;
};
