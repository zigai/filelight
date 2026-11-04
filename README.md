# Filelight

Filelight allows you to understand exactly where your diskspace is being used by
graphically representating your filesystem as a set of concentric
segmented-rings.

# Useful Tips

- Click on directories to center the view there
- Click on files to launch the application associated with them
- Middle click to open files/directories with your file browser
- Increase the window size to see more detail
- Hover over segments to find out more information
- You can specify a directory to scan on startup from the command line like this: `filelight /home/me/foobar`

# Bug Reporting

There are less bugs everyday! Please report bugs at https://bugs.kde.org/

# Enjoy!

We hope you enjoy using Filelight!

Martin Sandsmark <martin.sandsmark@kde.org>

Max Howell <max.howell@methylblue.com>

## Fork changes

- **Clipboard:** copies plain-text paths as well as file references for pasting into file managers.
- **Multi-selection:** Ctrl/Shift-click to select folders and files, then copy or delete the selection. Deletion is still permanent, with confirmation.
- **Sidebar:** tighter rows and a draggable divider that remembers its width. Single-click selects; double-click opens.
- **Navigation:** mouse Back/Forward and Alt+Left/Right retrace folder visits. Back falls back to the parent when there's no earlier visit; Forward can return. Up / Alt+Up always goes to the parent.
