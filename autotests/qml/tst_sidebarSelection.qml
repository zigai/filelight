// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2026 zigai

import QtQuick
import QtTest
import "../../src/qml" as UI

TestCase {
    name: "SidebarSelection"

    UI.SidebarSelection {
        id: selection
        rowCount: 8
    }

    function init() { selection.clear(); }

    function test_plainClick() {
        selection.select(2, Qt.NoModifier);
        compare(selection.rows, [2]);
        selection.select(5, Qt.NoModifier);
        compare(selection.rows, [5]);
        compare(selection.anchor, 5);
    }

    function test_controlToggle() {
        selection.select(1, Qt.NoModifier);
        selection.select(4, Qt.ControlModifier);
        compare(selection.rows, [1, 4]);
        selection.select(1, Qt.ControlModifier);
        compare(selection.rows, [4]);
        selection.select(4, Qt.ControlModifier);
        compare(selection.rows, []);
    }

    function test_shiftRangeAndShrink() {
        selection.select(2, Qt.NoModifier);
        selection.select(6, Qt.ShiftModifier);
        compare(selection.rows, [2, 3, 4, 5, 6]);
        selection.select(4, Qt.ShiftModifier);
        compare(selection.rows, [2, 3, 4]);
        selection.select(0, Qt.ShiftModifier);
        compare(selection.rows, [0, 1, 2]);
        compare(selection.anchor, 2);
    }

    function test_controlShiftRange() {
        selection.select(0, Qt.NoModifier);
        selection.select(4, Qt.ControlModifier);
        selection.select(6, Qt.ControlModifier | Qt.ShiftModifier);
        compare(selection.rows, [0, 4, 5, 6]);
    }

    function test_rightClickPreservesSelection() {
        selection.select(1, Qt.NoModifier);
        selection.select(3, Qt.ControlModifier);
        selection.contextSelect(1);
        compare(selection.rows, [1, 3]);
        selection.contextSelect(6);
        compare(selection.rows, [6]);
    }

    function test_keyboardNavigation() {
        selection.navigate(1, Qt.NoModifier);
        selection.navigate(2, Qt.ShiftModifier);
        compare(selection.rows, [1, 2]);
        selection.navigate(5, Qt.ControlModifier);
        compare(selection.rows, [1, 2]);
        compare(selection.current, 5);
        selection.navigate(100, Qt.NoModifier);
        compare(selection.rows, [7]);
    }

    function test_selectAllAndReset() {
        selection.selectAll();
        compare(selection.rows, [0, 1, 2, 3, 4, 5, 6, 7]);
        selection.clear();
        compare(selection.rows, []);
        compare(selection.anchor, -1);
        compare(selection.current, -1);
    }

    function test_invalidRows() {
        selection.select(-1, Qt.NoModifier);
        selection.select(8, Qt.ControlModifier);
        selection.contextSelect(99);
        compare(selection.rows, []);
    }

    function test_shiftWithoutAnchor() {
        selection.select(3, Qt.ShiftModifier);
        compare(selection.rows, [3]);
    }
}
