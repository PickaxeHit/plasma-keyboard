// SPDX-FileCopyrightText: 2026 PickaxeHit <pickaxehit@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import org.kde.plasma.keyboard.layouts.defaultplugin as SharedLayouts
import org.kde.plasma.keyboard.virtualkeyboard
import org.kde.plasma.keyboard.virtualkeyboard.components
import "shared" as Shared

// Double pinyin (小鹤), with the scheme fixed by the layout itself.
KeyboardLayoutDefinition {
    textComposer: PinyinTextComposer {
        pinyinScheme: "xhe"
        pinyinSchemeLocked: true
    }

    symbols: Shared.Symbols {}

    SharedLayouts.Desktop {
        inputModeKeyModes: [
            { value: "pinyin", label: "中" },
            { value: "latin", label: "英" }
        ]
    }
}
