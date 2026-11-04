# Folder Back and Forward history

The active map now has real per-window folder navigation history:

- Mouse Back / Alt+Left returns to the previously visited folder.
- Mouse Forward / Alt+Right returns to the folder left by Back.
- If the window was opened directly on a folder and has no earlier visit,
  Back still goes up to its parent, preserving the previous behavior. Forward
  returns to the original folder. Further parent fallbacks preserve the whole
  Forward chain.
- The Up toolbar action / Alt+Up always goes to the parent folder, independent
  of history, and records that navigation as a normal visit.
- Opening a different folder after Back discards the old Forward branch.
  Rescanning/reopening the same location does not duplicate it or clear Forward.
- At either history boundary the unavailable action does nothing. Back can
  still fall back to the parent at the oldest entry; at the filesystem root it
  does nothing if there is no earlier visit.
- Back and Forward are also available in the map's overflow actions. They do
  not switch to Overview. Other windows/overlay pages keep normal behavior.

History is kept in memory, not saved across application restarts. It stores
URLs, not cached trees or sidebar indexes. Directory URL normalization treats
trailing slashes and normalized path segments as the same folder while
preserving Unicode and URL-sensitive filenames.

History records accepted folder-open requests. URLs/local folders rejected
before scanning (including deleted Back/Forward targets) do not move the cursor
or discard the Forward branch. As with the existing immediate location update,
an asynchronous remote/network listing error may retain its accepted entry;
history does not transactionally roll back recursive scans.

## Verification

All nine CTest suites pass. The new history suite covers empty/boundary states,
multiple steps, branch replacement, duplicate/normalized URLs, initial Back-Up
reversal, and special/remote URL storage. Backend integration uses real
throwaway scan folders to test chart/sidebar-style `updateURL` plus `openUrl`,
rescanning with Forward available, failed replay of a deleted folder followed by
retry, rejected new visits, and Up versus history travel.

Input tests cover native mouse Forward press/release, Alt+Right and native
Forward keys, shortcut claiming, repeat suppression, disabled mode, and no
accidental Back signal.

An isolated KWin GUI session verified the initial `A → Back → parent → Forward
→ A` round trip, repeated travel, Forward after F5, and opening B after Back
replacing the old A branch. Subsequent Back/Forward returned between B and its
parent without leaving the map.

Build/install using [the local build instructions](CLIPBOARD_FIX.md). Reopen
Filelight after installing to activate the new binary.
