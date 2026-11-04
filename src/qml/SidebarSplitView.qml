// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2026 zigai

import QtQuick
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami

QQC2.SplitView {
    orientation: Qt.Horizontal

    handle: Rectangle {
        objectName: "sidebarResizeHandle"
        implicitWidth: Math.max(6, Kirigami.Units.smallSpacing * 2)
        color: QQC2.SplitHandle.pressed ? Kirigami.Theme.highlightColor
            : QQC2.SplitHandle.hovered ? Kirigami.Theme.alternateBackgroundColor
            : Kirigami.Theme.backgroundColor

        Rectangle {
            anchors.centerIn: parent
            width: 1
            height: parent.height
            color: Kirigami.Theme.disabledTextColor
            opacity: 0.4
        }

        HoverHandler {
            cursorShape: Qt.SplitHCursor
        }
    }
}
