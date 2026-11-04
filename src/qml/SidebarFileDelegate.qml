// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2026 zigai

import org.kde.kirigami as Kirigami
import org.kde.kirigami.delegates as KD

KD.SubtitleDelegate {
    // Keep both text lines, but avoid the desktop style's large row insets.
    topPadding: Kirigami.Units.smallSpacing
    bottomPadding: Kirigami.Units.smallSpacing
    topInset: 0
    bottomInset: 0
    icon.width: Kirigami.Units.iconSizes.smallMedium
    icon.height: Kirigami.Units.iconSizes.smallMedium
}
