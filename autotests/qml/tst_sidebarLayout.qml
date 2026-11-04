// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2026 zigai

import QtQuick
import QtQuick.Controls as QQC2
import QtTest
import org.kde.kirigami as Kirigami
import "../../src/qml" as UI

TestCase {
    id: testCase
    name: "SidebarLayout"
    visible: true
    width: 800
    height: 400
    when: windowShown

    UI.SidebarSplitView {
        id: split
        anchors.fill: parent
        Rectangle {
            id: sidebar
            QQC2.SplitView.minimumWidth: 144
            QQC2.SplitView.preferredWidth: 260
        }
        Rectangle {
            id: chart
            QQC2.SplitView.minimumWidth: 180
            QQC2.SplitView.fillWidth: true
        }
    }

    UI.SidebarFileDelegate {
        id: compactRow
        visible: false
        width: 260
        text: "Folder"
        subtitle: "1.0 GiB"
    }

    function init() {
        sidebar.visible = true;
        sidebar.QQC2.SplitView.preferredWidth = 260;
        testCase.width = 800;
        wait(20);
    }

    function test_compactRow() {
        compare(compactRow.topInset, 0);
        compare(compactRow.bottomInset, 0);
        compare(compactRow.topPadding, Kirigami.Units.smallSpacing);
        compare(compactRow.bottomPadding, Kirigami.Units.smallSpacing);
        compare(compactRow.icon.width, Kirigami.Units.iconSizes.smallMedium);
        compare(compactRow.icon.height, compactRow.icon.width);
        verify(compactRow.implicitHeight >= compactRow.contentItem.implicitHeight);
    }

    function test_dragDivider() {
        const handle = findChild(split, "sidebarResizeHandle");
        verify(handle !== null);
        const previous = sidebar.width;
        mouseDrag(handle, handle.width / 2, handle.height / 2, 100, 0, Qt.LeftButton);
        tryVerify(() => sidebar.width > previous + 60);
        verify(chart.width >= 180);
        mouseDrag(handle, handle.width / 2, handle.height / 2, -700, 0, Qt.LeftButton);
        tryCompare(sidebar, "width", 144);
        verify(chart.width >= 180);
    }

    function test_hiddenSidebar() {
        sidebar.visible = false;
        tryVerify(() => Math.abs(chart.width - split.width) < 1);
        sidebar.visible = true;
        tryVerify(() => sidebar.width >= 144 && chart.width >= 180);
    }
}
