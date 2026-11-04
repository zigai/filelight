// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2026 zigai

import QtQuick

QtObject {
    property int rowCount: 0
    property var rows: []
    property int anchor: -1
    property int current: -1

    function contains(row) {
        return rows.indexOf(row) !== -1;
    }

    function clear() {
        rows = [];
        anchor = -1;
        current = -1;
    }

    function select(row, modifiers) {
        if (row < 0 || row >= rowCount) {
            return;
        }
        const control = (modifiers & Qt.ControlModifier) !== 0;
        const shift = (modifiers & Qt.ShiftModifier) !== 0;
        let next = [];
        if (shift) {
            if (anchor < 0) {
                anchor = current >= 0 ? current : row;
            }
            next = control ? rows.slice() : [];
            for (let i = Math.min(anchor, row); i <= Math.max(anchor, row); ++i) {
                if (next.indexOf(i) === -1) {
                    next.push(i);
                }
            }
        } else if (control) {
            next = rows.slice();
            const position = next.indexOf(row);
            if (position === -1) {
                next.push(row);
            } else {
                next.splice(position, 1);
            }
            anchor = row;
        } else {
            next = [row];
            anchor = row;
        }
        rows = next.sort((a, b) => a - b);
        current = row;
    }

    function contextSelect(row) {
        if (row < 0 || row >= rowCount) {
            return;
        }
        if (!contains(row)) {
            select(row, Qt.NoModifier);
        } else {
            current = row;
        }
    }

    function selectAll() {
        let next = [];
        for (let i = 0; i < rowCount; ++i) {
            next.push(i);
        }
        rows = next;
        if (rowCount > 0 && current < 0) {
            current = 0;
        }
        anchor = current;
    }

    function navigate(row, modifiers) {
        if (rowCount === 0) {
            return;
        }
        row = Math.max(0, Math.min(rowCount - 1, row));
        if ((modifiers & Qt.ControlModifier) && !(modifiers & Qt.ShiftModifier)) {
            current = row;
        } else {
            select(row, modifiers);
        }
    }
}
