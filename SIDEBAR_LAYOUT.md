# Compact, resizable sidebar and parent-folder Back

The sidebar keeps the folder name and size on two lines, but uses small vertical
padding, no large style insets, and proportionate small-medium icons. Selection
and its modifier-click behavior are unchanged.

Drag the divider between the list and chart to resize either pane. Both have
minimum widths so neither disappears; hiding the sidebar still gives the chart
the whole content area. The sidebar defaults to 320 logical pixels and saves its
width after a completed drag in `filelightrc`, group `filelight_part`, key
`foldersSidebarWidth`. It is restored on restart, clamped to the current window's
available space by SplitView. Chart hit testing, context menus and drops stay
local to the chart canvas after moving the divider.

## Back navigation

On the map, the mouse Back button and the platform Back key/shortcut (including
Alt+Left) go up one folder, like the existing Up toolbar action / Alt+Up. At the
filesystem root or while the map is busy, they are consumed without switching to
Overview. Native popup menus retain their platform dismissal behavior.

The original reset-like behavior was Kirigami switching from Map to Overview,
not discarding the scan. Page-level Back cancellation did not reliably prevent
the fallback on the installed Qt/Kirigami stack. `NavigationInput` therefore
filters the active window's ShortcutOverride and Back key/button events before
the generic page-stack shortcut handles them. It is enabled only for the active
map without an overlay page; Overview and other dialogs keep their behavior.
Keyboard auto-repeat is consumed without producing extra navigation requests.
No global mouse or desktop key mapping is changed.

## Verification

- All eight CTest suites pass, including window-level Back key/button tests
  (press/release, ShortcutOverride, auto-repeat, disabled mode, unrelated keys,
  and replacing the window).
- The QML layout tests drag the divider, check minimum sizes and sidebar hiding,
  and check the compact delegate's padding/insets/icon proportions.
- In an isolated KWin session, dragging from 320 to 440 pixels wrote the saved
  setting, and closing/reopening the app restored the divider at 440 pixels.
- Actual Alt+Left navigation moved from a test folder to its parent and stayed
  on the map. Multi-selection/copy and chart context-copy continued working
  after resizing.

Build/install using [the local build instructions](CLIPBOARD_FIX.md). Reopen
existing Filelight windows to use the update.
