# Sidebar multi-selection

The folder/file sidebar now behaves like a conventional file manager:

- Click selects a single row; double-click or Enter opens it (folders center
  the map, files open externally).
- Ctrl-click toggles individual rows. Shift-click selects the range from the
  anchor; Ctrl+Shift-click adds a range without dropping the existing selection.
- Ctrl+A selects all sidebar rows. Escape or clicking empty sidebar space clears
  the selection. Arrow/Home/End keys move selection, Shift extends it, and Ctrl
  moves focus without changing selection. Space toggles the focused row.
- Hovering never changes the selection or its range anchor.
- Right-clicking an already selected row keeps the whole selection. Right-clicking
  another row selects that row alone.
- Copy and Delete context-menu actions operate on all selected rows. Open opens
  all selected URLs externally. Single-folder-only actions are hidden for multiple
  rows. Ctrl+C copies all selected paths, one per line, while retaining the URL
  list and portal file-copy payload.
- Delete requests one confirmation for the complete selection. **Deletion remains
  permanent, not Move to Trash**, matching the existing Filelight action. Successful
  entries are removed from the tree, failures are reported, and the view refreshes
  only after the batch completes. Cancelling changes nothing.
- Navigation, rescanning and model resets clear selection so old row indexes
  cannot act on different files.

This applies to the sidebar, not multi-selecting wedges in the radial chart.
The chart retains its existing single-segment interactions.

Build/install using [the local build instructions](CLIPBOARD_FIX.md). Restart
Filelight after installing an update.

## Tests

The seventh CTest suite exercises the standalone QML selection controller:
Ctrl toggling, Shift range growth/shrink and reverse ranges, additive ranges,
right-click preservation/replacement, keyboard navigation, select-all/reset,
invalid rows and an initially unset anchor. C++ tests cover multiple clipboard
URLs and newline-separated paths, empty copies, invalid/duplicate row indexes,
empty deletion requests, and notified batch model removals with safe stale-index
access. Cancelling a bulk deletion is checked in the GUI session (the platform
dialog is not mocked in headless tests).

An isolated KWin Wayland session was used to test actual mouse/keyboard
interaction, copying through both the context menu and Ctrl+C, Ctrl+A,
Shift+Down, Space toggling, Enter, empty-area clearing, and double-click folder
navigation. Button-like delegates forward keys to the ListView so Space/Enter
are not consumed before the selection handlers run. A destructive test used only
throwaway build-directory fixtures: the confirmation named both selected
folders, deleting them left all three unselected siblings untouched, and
selection cleared after the model refreshed.
