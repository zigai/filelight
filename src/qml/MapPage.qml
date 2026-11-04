// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2022 Harald Sitter <sitter@kde.org>

import QtQuick
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.coreaddons as KCoreAddons
import QtQuick.Shapes

import org.kde.filelight

Kirigami.Page {
    id: page

    property url url: RadialMap.rootUrl
    property QQC2.Menu contextMenu

    ContextMenuContext{
        id: contextMenuContext
    }

    enabled: !contextMenuContext.deleting
    topPadding: 0
    leftPadding: 0
    rightPadding: 0
    bottomPadding: 0

    Connections {
        target: contextMenuContext
        function onDeleteFileFailed(reason) {
            showPassiveNotification(reason);
        }
    }

    footer: QQC2.ToolBar {
        height: footerLabel.implicitHeight + topPadding + bottomPadding
        visible: shapeItem.visible
        contentItem: QQC2.Label {
            id: footerLabel
            verticalAlignment: Text.AlignVCenter
            text: {
                const shown = RadialMap.numberOfChildren === 0
                    ? i18nc("@info:status", "No files shown.")
                    : i18ncp("@info:status", "1 file shown", "%1 files shown", RadialMap.numberOfChildren);
                return sidebarSelection.rows.length > 0
                    ? shown + " — " + i18ncp("@info:status", "1 selected", "%1 selected", sidebarSelection.rows.length)
                    : shown;
            }
        }
    }

    Kirigami.Action {
        id: goToOverviewAction
        enabled: page.state === ""
        icon.name: "go-home"
        text: i18nc("@action", "Go to Overview")
        onTriggered: pageStack.currentIndex = 0
    }

    Kirigami.Action {
        id: goUpAction
        enabled: page.state === "" && MainContext.upEnabled
        icon.name: "go-parent-folder"
        text: i18nc("@action", "Up")
        onTriggered: appWindow.slotUp()
        shortcut: "Alt+Up"
    }

    Kirigami.Action {
        id: goBackAction
        enabled: page.state === "" && MainContext.backEnabled
        icon.name: "go-previous"
        text: i18nc("@action", "Back")
        displayHint: Kirigami.DisplayHint.AlwaysHide
        onTriggered: MainContext.slotBack()
    }

    Kirigami.Action {
        id: goForwardAction
        enabled: page.state === "" && MainContext.forwardEnabled
        icon.name: "go-next"
        text: i18nc("@action", "Forward")
        displayHint: Kirigami.DisplayHint.AlwaysHide
        onTriggered: MainContext.slotForward()
    }

    NavigationInput {
        window: appWindow
        enabled: appWindow.pageStack.currentIndex > 0
                 && appWindow.pageStack.currentItem === page
                 && appWindow.pageStack.layers.depth <= 1
        onBackRequested: {
            if (page.enabled && goBackAction.enabled) goBackAction.trigger();
        }
        onForwardRequested: {
            if (page.enabled && goForwardAction.enabled) goForwardAction.trigger();
        }
    }

    Kirigami.Action {
        id: rescanAction
        enabled: page.state !== "scanning"
        icon.name: "view-refresh"
        text: i18nc("@action", "Rescan")
        onTriggered: appWindow.rescan()
        shortcut: StandardKey.Refresh
    }

    Kirigami.Action {
        id: stopAction
        enabled: page.state === "scanning"
        icon.name: "process-stop"
        text: i18nc("@action", "Stop")
        onTriggered: appWindow.closeURL()
    }

    Kirigami.Action {
        id: zoomInAction
        enabled: page.state === ""
        icon.name: "zoom-in"
        text: i18nc("@action", "Zoom In")
        displayHint: Kirigami.DisplayHint.AlwaysHide
        onTriggered: RadialMap.zoomIn()
        shortcut: StandardKey.ZoomIn
    }

    Kirigami.Action {
        id: zoomOutAction
        enabled: page.state === ""
        icon.name: "zoom-out"
        text: i18nc("@action", "Zoom Out")
        displayHint: Kirigami.DisplayHint.AlwaysHide
        onTriggered: RadialMap.zoomOut()
        shortcut: StandardKey.ZoomOut
    }

    actions: [
        goUpAction,
        goBackAction,
        goForwardAction,
        goToOverviewAction,
        rescanAction,
        stopAction,
        zoomInAction,
        zoomOutAction,
        configureAction,
        helpAction,
        aboutAction
    ]

    Component {
        id: contextMenuComponent
        QQC2.Menu {
            id: contextMenu

            required property Segment segment

            title: segment.displayName()
            visible: !segment.isFilesGroup

            onAboutToShow: page.contextMenu = this
            onAboutToHide: page.contextMenu = null

            QQC2.MenuItem {
                action: Kirigami.Action {
                    icon.name: "document-open"
                    text: i18nc("@action Open file or directory from context menu", "Open")
                }
                onTriggered: Qt.openUrlExternally(contextMenu.segment.url())
            }
            QQC2.MenuItem {
                visible: contextMenu.segment.isFolder() && contextMenu.segment.url().toString().startsWith("file:")
                action: Kirigami.Action {
                    icon.name: "utilities-terminal"
                    text: i18nc("@action", "Open Terminal Here")
                    onTriggered: contextMenuContext.openTerminal(contextMenu.segment.url())
                }
            }
            QQC2.MenuItem {
                visible: contextMenu.segment.isFolder()
                action: Kirigami.Action {
                    icon.name: "zoom-in"
                    text: i18nc("@action focuses the filelight view on a given map segment", "Center Map Here")
                    onTriggered: {
                        MainContext.updateURL(contextMenu.segment.url())
                        MainContext.openUrl(contextMenu.segment.url())
                    }
                }
            }
            QQC2.MenuItem {
                visible: contextMenu.segment.isFolder()
                action: Kirigami.Action {
                    icon.name: "list-remove"
                    text: i18nc("@action", "Add to Do Not Scan List")
                    onTriggered: contextMenuContext.doNotScan(contextMenu.segment.url())
                }
            }
            QQC2.MenuItem {
                visible: contextMenu.segment.isFolder()
                action: Kirigami.Action {
                    icon.name: "view-refresh"
                    text: i18nc("@action rescan filelight map", "Rescan")
                    onTriggered: MainContext.rescanSingleDir(contextMenu.segment.url())
                }
            }
            QQC2.MenuItem {
                action: Kirigami.Action {
                    icon.name: "edit-copy"
                    text: i18nc("@action", "Copy to clipboard")
                    onTriggered: contextMenuContext.copyClipboard(contextMenu.segment.url())
                }
            }
            QQC2.MenuSeparator {}
            QQC2.MenuItem {
                action: Kirigami.Action {
                    icon.name: "edit-delete"
                    text: i18nc("@action delete file or folder", "Delete")
                    onTriggered: contextMenuContext.deleteFileFromSegment(contextMenu.segment)
                }
            }
        }
    }

    property real mouseyX: -1
    property real mouseyY: -1
    property var hoveredSegment: undefined
    property bool hoveringListItem: false

    SidebarSelection {
        id: sidebarSelection
        rowCount: listview.count
    }

    Connections {
        target: FileModel
        function onModelReset() {
            sidebarMenu.close();
            sidebarSelection.clear();
        }
    }

    QQC2.Menu {
        id: sidebarMenu
        property var rows: []
        property var urls: []
        property bool singleFolder: false

        onAboutToShow: page.contextMenu = this
        onAboutToHide: page.contextMenu = null
        onClosed: {
            if (page.visible && page.enabled) listview.forceActiveFocus();
        }

        QQC2.MenuItem {
            icon.name: "document-open"
            text: i18nc("@action Open selected files or directories", "Open")
            onTriggered: {
                for (const url of sidebarMenu.urls) Qt.openUrlExternally(url);
            }
        }
        QQC2.MenuItem {
            visible: sidebarMenu.singleFolder && sidebarMenu.urls.length === 1
                     && sidebarMenu.urls[0].toString().startsWith("file:")
            icon.name: "utilities-terminal"
            text: i18nc("@action", "Open Terminal Here")
            onTriggered: contextMenuContext.openTerminal(sidebarMenu.urls[0])
        }
        QQC2.MenuItem {
            visible: sidebarMenu.singleFolder
            icon.name: "list-remove"
            text: i18nc("@action", "Add to Do Not Scan List")
            onTriggered: contextMenuContext.doNotScan(sidebarMenu.urls[0])
        }
        QQC2.MenuItem {
            icon.name: "edit-copy"
            text: i18nc("@action", "Copy to clipboard")
            onTriggered: contextMenuContext.copyUrls(sidebarMenu.urls)
        }
        QQC2.MenuSeparator {}
        QQC2.MenuItem {
            icon.name: "edit-delete"
            text: i18nc("@action delete selected files or folders", "Delete")
            onTriggered: contextMenuContext.deleteRows(sidebarMenu.rows)
        }
    }

    SidebarSplitView {
        id: mapSplitView
        objectName: "mapSplitView"
        anchors.fill: parent
        visible: page.state === ""

        onResizingChanged: {
            if (!resizing && foldersSidebar.visible) {
                Config.foldersSidebarWidth = Math.round(foldersSidebar.width);
                Config.write();
            }
        }

        QQC2.ScrollView {
            id: foldersSidebar
            objectName: "foldersSidebar"
            visible: Config.showFoldersSidebar
            QQC2.SplitView.minimumWidth: Kirigami.Units.gridUnit * 8
            QQC2.SplitView.preferredWidth: Config.foldersSidebarWidth
            Kirigami.Theme.colorSet: Kirigami.Theme.View
            Kirigami.Theme.inherit: false

            // flush against both the toolbar and the window edge. without this we get a framed rectangle
            background: Rectangle {
	             color: Kirigami.Theme.backgroundColor
	        }
            Component.onCompleted: background.visible = true

            ListView {
                id: listview
                objectName: "folderList"
                clip: true
                model: visible ? FileModel : undefined
                currentIndex: sidebarSelection.current
                reuseItems: true
                focus: true
                activeFocusOnTab: true
                keyNavigationEnabled: false

                Keys.onPressed: event => {
                    if (event.matches(StandardKey.SelectAll)) {
                        sidebarSelection.selectAll();
                    } else if (event.matches(StandardKey.Copy)) {
                        contextMenuContext.copyUrls(FileModel.urls(sidebarSelection.rows));
                    } else if (event.key === Qt.Key_Escape) {
                        sidebarSelection.clear();
                    } else if (event.key === Qt.Key_Delete) {
                        contextMenuContext.deleteRows(sidebarSelection.rows);
                    } else if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                        const urls = FileModel.urls(sidebarSelection.rows);
                        if (urls.length === 1) {
                            if (FileModel.isFolder(sidebarSelection.rows[0])) {
                                MainContext.updateURL(urls[0]);
                                MainContext.openUrl(urls[0]);
                            } else {
                                Qt.openUrlExternally(urls[0]);
                            }
                        }
                    } else if (event.key === Qt.Key_Space && sidebarSelection.current >= 0) {
                        sidebarSelection.select(sidebarSelection.current, event.modifiers | Qt.ControlModifier);
                    } else if ([Qt.Key_Up, Qt.Key_Down, Qt.Key_Home, Qt.Key_End].indexOf(event.key) !== -1) {
                        let row = sidebarSelection.current;
                        if (event.key === Qt.Key_Home) row = 0;
                        else if (event.key === Qt.Key_End) row = listview.count - 1;
                        else if (event.key === Qt.Key_Up) row = row < 0 ? listview.count - 1 : row - 1;
                        else row = row < 0 ? 0 : row + 1;
                        sidebarSelection.navigate(row, event.modifiers);
                        listview.positionViewAtIndex(sidebarSelection.current, ListView.Contain);
                    } else {
                        event.accepted = false;
                        return;
                    }
                    event.accepted = true;
                }

                TapHandler {
                    acceptedButtons: Qt.LeftButton
                    onTapped: eventPoint => {
                        if (listview.indexAt(eventPoint.position.x + listview.contentX,
                                             eventPoint.position.y + listview.contentY) < 0) {
                            sidebarSelection.clear();
                            listview.forceActiveFocus();
                        }
                    }
                }

                delegate: SidebarFileDelegate {
                    id: sidebarDelegate
                    width: listview.width
                    icon.name: ROLE_IsFolder ? "folder" : "file" // TODO mimetype?
                    text: model.display
                    subtitle: ROLE_HumanReadableSize
                    hoverEnabled: !page.contextMenu
                    highlighted: sidebarSelection.contains(index)
                    Accessible.selectable: true
                    Accessible.selected: highlighted
                    // Keyboard focus is separate from selection (Ctrl+arrows).
                    focus: listview.currentIndex === index
                    // Button-like delegates otherwise consume Space/Enter
                    // before the ListView's selection handlers can see them.
                    Keys.forwardTo: [listview]

                    function openItem() {
                        if (ROLE_IsFolder) {
                            MainContext.updateURL(ROLE_URL);
                            MainContext.openUrl(ROLE_URL);
                        } else {
                            Qt.openUrlExternally(ROLE_URL);
                        }
                    }

                    onHoveredChanged: {
                        if (hovered) {
                            hoveringListItem = true;
                            hoveredSegment = ROLE_Segment;
                        }
                    }

                    TapHandler {
                        acceptedButtons: Qt.LeftButton
                        onTapped: (eventPoint, button) => {
                            listview.forceActiveFocus();
                            sidebarSelection.select(index, point.modifiers);
                        }
                        onDoubleTapped: (eventPoint, button) => {
                            if (!(point.modifiers & (Qt.ControlModifier | Qt.ShiftModifier))) {
                                sidebarDelegate.openItem();
                            }
                        }
                    }

                    TapHandler {
                        acceptedButtons: Qt.RightButton
                        onTapped: {
                            listview.forceActiveFocus();
                            sidebarSelection.contextSelect(index);
                            sidebarMenu.rows = sidebarSelection.rows.slice();
                            sidebarMenu.urls = FileModel.urls(sidebarMenu.rows);
                            sidebarMenu.singleFolder = sidebarMenu.rows.length === 1 && ROLE_IsFolder;
                            sidebarMenu.popup();
                        }
                    }
                }
            }
        }

        Item {
            id: chartPane
            QQC2.SplitView.fillWidth: true
            QQC2.SplitView.minimumWidth: Kirigami.Units.gridUnit * 10

            Item {
                id: shapeItem
                objectName: "mapCanvas"
                anchors.fill: parent
                anchors.margins: Kirigami.Units.gridUnit
                antialiasing: true
                layer.enabled: true
                layer.samples: 8
                layer.smooth: true

                property var zOrderedShapes: []
                property bool hasShapes: zOrderedShapes.length > 0

                function objectToArray(object) {
                    let newArray = [];
                    for (let i in object) {
                        newArray.push(object[i]);
                    }
                    return newArray;
                }

                onChildrenChanged: {
                    let children = objectToArray(shapeItem.children)
                    children = children.filter(child => child instanceof Shape);
                    children.sort((a, b) => b.z - a.z); // sort by level so the higher level object (the visible one) outscores ones it paints over
                    zOrderedShapes = children
                }

                function tooltip({ path, size, isFolder, files, totalFiles, isRoot = false }) {
                    let tips = [path, size]
                    if (isFolder) {
                        const percent = Math.floor((100 * files) / totalFiles)
                        if (percent > 0) {
                            tips.push(i18ncp("Tooltip of folder, %1 is number of files", "%1 File (%2%)", "%1 Files (%2%)", files, percent))
                        } else {
                            tips.push(i18ncp("Tooltip of folder, %1 is number of files", "%1 File", "%1 Files", files))
                        }
                    }
                    if (isRoot) {
                        tips.push(i18nc("part of tooltip indicating that the item under the mouse is clickable", "Click to go up to parent directory"))
                    }
                    return tips.join("\n")
                }

                Instantiator {
                    id: instantiator
                    model: shapeItem.visible ? RadialMap.signature : undefined
                    active: true
                    asynchronous: true // arguable

                    Instantiator {
                        // FIXME weird
                        Component.onCompleted: idx = index // lock index by breaking the binding
                        property int idx: index
                        readonly property real signatureRadius: {
                            if (shapeItem.width > shapeItem.height) {
                                return shapeItem.height / 2 / (instantiator.model.length + 1)
                            }
                            return shapeItem.width / 2 / (instantiator.model.length + 1)
                        }
                        readonly property real shapeRadius: signatureRadius * (idx + 2)

                        active: true
                        model: modelData
                        onObjectAdded: (index, object) => shapeItem.children.push(object)

                        delegate: SegmentShape {
                            // Qt doc: Note: model, index, and modelData roles are not accessible if the delegate contains required properties, unless it has also required properties with matching names.
                            required property var modelData

                            readonly property bool segmentHover: hoveredSegment === segment.uuid

                            z: (instantiator.model.length - idx) // reverse order such that more central levels are above. this gives us segment appearance without having to actually paint segments (instead we stack full cicles)
                            segment: modelData
                            item: shapeItem
                            radius: shapeRadius
                            startAngle: -(modelData.start() / 16)
                            sweepAngle: -(modelData.length() / 16)
                            tooltipText: shapeItem.tooltip({
                                isFolder: segment.isFolder(),
                                path: segment.displayPath(),
                                size: segment.humanReadableSize(),
                                files: segment.files(),
                                totalFiles: RadialMap.numberOfChildren,
                            })
                            showTooltip: !contextMenu && !hoveringListItem && segmentHover && shapeItem.visible
                            fillColor: segmentHover ? Qt.darker(segment.color) : segment.color
                        }
                    }
                }

                CenterShape {
                    id: centerShape
                    z: 500 // on top of everything, arbitrary high number of shapes
                    visible: shapeItem.hasShapes
                    segment: RadialMap.rootSegment
                    segmentUuid: "root"
                    item: shapeItem
                    radius: {
                        if (instantiator.model === undefined) {
                            return 0
                        }
                        if (shapeItem.width > shapeItem.height) {
                            return shapeItem.height / 2 / (instantiator.model.length + 1)
                        }
                        return shapeItem.width / 2 / (instantiator.model.length + 1)
                    }
                    startAngle: 0
                    sweepAngle: 360
                    tooltipText: shapeItem.tooltip({
                        isFolder: true,
                        path: RadialMap.displayPath,
                        size: RadialMap.overallSize,
                        files: RadialMap.numberOfChildren,
                        totalFiles: RadialMap.numberOfChildren,
                        isRoot: true,
                    })
                    showTooltip: !contextMenu && !hoveringListItem && hoveredSegment === segmentUuid && shapeItem.visible
                    Kirigami.Theme.colorSet: Kirigami.Theme.View
                    fillColor: Kirigami.Theme.backgroundColor
                }

                QQC2.Label {
                    id: centerLabel
                    z: 501
                    visible: centerShape.visible
                    text: RadialMap.overallSize
                    Kirigami.Theme.colorSet: Kirigami.Theme.View
                    color: Kirigami.Theme.textColor
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    // The diagonal of the circle is the hypotenuse of the largest square
                    readonly property real dimension: (2  * centerShape.radius) / Math.sqrt(2)
                    width: dimension
                    height: dimension
                    anchors.centerIn: parent

                    // Let the text scale way down but lock the maximum at the actual default font height.
                    // This ensures that the text neatly fits into our dimensions.
                    fontSizeMode: Text.Fit
                    minimumPixelSize: 2
                    Component.onCompleted: font.pixelSize = font.pixelSize
                }

                Kirigami.PlaceholderMessage {
                    id: openUrlWarning
                    visible: !centerShape.visible
                    anchors.centerIn: parent
                    width: parent.width - (Kirigami.Units.largeSpacing * 4)
                    Connections {
                        target: MainContext
                        function onOpenUrlFailed(text, explanation) {
                            openUrlWarning.text = text;
                            openUrlWarning.explanation = explanation;
                        }
                    }
                }
            }
        }
    }

    MouseArea {
        parent: shapeItem
        anchors.fill: parent
        z: 502
        hoverEnabled: !page.contextMenu
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        function findTarget(mouse) {
            let children = shapeItem.zOrderedShapes
            for (var i in children) {
                const child = children[i]
                if (child === centerLabel) {
                    continue // not part of the shape objects
                }
                const contains = child.contains(Qt.point(mouse.x, mouse.y))
                if (contains) {
                    return child
                }
            }
            return null
        }

        onPositionChanged: mouse => {
            mouse.accepted = false
            const child = findTarget(mouse)
            if (child !== null) {
                hoveringListItem = false
                mouseyX = mouse.x
                mouseyY = mouse.y
                if (child.segmentUuid === undefined) {
                    hoveredSegment = "root"
                } else {
                    hoveredSegment = child.segmentUuid
                }
            } else {
                hoveredSegment = undefined
            }
        }

        onClicked: mouse => {
            const child = findTarget(mouse)
            if (child === null) {
                return
            }
            if (mouse.button === Qt.LeftButton) {
                if (child.segmentUuid === "root") {
                    appWindow.slotUp()
                    return
                }
                if (child.segment.isFilesGroup) {
                    // Cannot open a files group. It describes any number of files.
                    return
                }
                if (!child.segment.isFolder()) {
                    Qt.openUrlExternally(child.url)
                    return
                }
                MainContext.updateURL(child.url)
                MainContext.openUrl(child.url)
            } else if (mouse.button === Qt.RightButton) {
                if (!child.segment.fake) {
                    contextMenuComponent.createObject(child, {segment: child.segment}).popup()
                }
            }
        }
        onPressAndHold: {
            if (mouse.source === Qt.MouseEventNotSynthesized) {
                const child = findTarget(mouse)
                contextMenu.segment = child.segment
                contextMenu.popup()
            }
        }
    }

    DropperItem {
        parent: shapeItem
        anchors.fill: parent
        z: 503
        onUrlsDropped: urls => {
            const url = urls[0]
            appWindow.updateURL(url)
            appWindow.openURL(url)
        }
    }

    Kirigami.LoadingPlaceholder {
        id: scanPlaceholder
        visible: page.state === "scanning"
        anchors.centerIn: parent

        Timer {
            interval: 16 // = 60 fps because supposedly Qt hardcodes it all over the place (this claim is very old and it's unclear if still true)
            // Polish doesn't lend itself to the advanced status text https://bugs.kde.org/show_bug.cgi?id=468395
            running: parent.visible && Qt.uiLanguage != 'pl'
            repeat: true
            onTriggered: {
                const files = ScanManager.files();
                const size = ScanManager.totalSize()
                scanPlaceholder.text = i18ncp("Scanned number of files and size so far", "%1 File, %2", "%1 Files, %2", String(files), KCoreAddons.Format.formatByteSize(size));
            }
        }
    }

    Kirigami.PlaceholderMessage {
        visible: page.state === "noData"
        anchors.centerIn: parent
        width: parent.width - (Kirigami.Units.largeSpacing * 4)
        text: i18n("No data available")
    }

    Connections {
        target: ScanManager
        function onAboutToEmptyCache() {
            RadialMap.invalidate()
        }
    }

    Connections {
        target: MainContext
        function onCanvasIsDirty(filth) {
            RadialMap.refresh(filth)
        }
    }

    Component.onCompleted: {
        appWindow.mapPage = this
    }

    states: [
        State {
            name: "scanning"
            when: ScanManager.running
        },
        State {
            name: "noData"
            // FIXME this toggles too soon causing incomplete shapes to show wtf - but only when instaniatior is no async
            when: !RadialMap.valid && shapeItem.hasShapes
        },
        State {
            name: "" // default state
        }
    ]
}
