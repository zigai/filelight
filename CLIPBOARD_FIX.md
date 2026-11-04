# Clipboard-fixed Filelight

This fork's `fix/clipboard-text-path` branch is based on KDE Filelight
`v26.08.2`, matching the workstation's installed distro version.

## Fix

The context-menu copy action now provides both the original URL list (for file
managers) and a plain-text path (for editors and terminals). Local paths are
unescaped filesystem paths; remote locations use their display URL.

The radial chart passes the segment's actual URL, not its display-path string,
so names containing spaces, Unicode, `#`, and `%` are not reinterpreted as URLs.

## Build, test, and install

Requires Qt 6, KDE Frameworks 6, extra-cmake-modules, CMake, Ninja, and a C++20
compiler. Keep the distro Filelight package installed for desktop integration
and as a fallback.

```sh
git switch fix/clipboard-text-path
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DCMAKE_INSTALL_PREFIX="$HOME/.local/opt/filelight" \
  -DBUILD_TESTING=ON
cmake --build build --parallel 4
QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure --timeout 45
cmake --install build
```

Close Filelight before running `cmake --install`, then reopen it afterward.
The workstation config repository manages
`~/.local/bin/filelight` and the desktop launcher override; the binary and
installation output are not dotfiles. The wrapper selects this installation,
falling back to `/usr/bin/filelight` when it is absent.

Distro updates do not replace this local installation. Update this fork
explicitly when adopting a newer KDE release, and rebuild after incompatible
Qt/KDE library updates. This is a local override, not a replacement pacman
package.

## Verification

- All eight CTest suites pass, including clipboard, [sidebar selection](MULTI_SELECTION.md),
  and [layout/navigation](SIDEBAR_LAYOUT.md) coverage.
- The new regression test fails on all four data rows when the plain-text
  addition is temporarily removed, and passes when restored.
- Installed binary tested in an isolated KWin Wayland session: right-click
  copy from both the folder list and the radial chart returns the correct
  plain-text path for `café #1 100%/sample.txt`.
- The same chart action retains the correctly encoded `text/uri-list` and
  portal file-transfer representation for file-manager pasting.
