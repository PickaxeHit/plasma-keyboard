// SPDX-FileCopyrightText: 2026 PickaxeHit <pickaxehit@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import org.kde.plasma.keyboard.virtualkeyboard
import org.kde.plasma.keyboard.virtualkeyboard.components
import "shared" as Shared

// Double pinyin (自然码), with the scheme fixed by the layout itself.
KeyboardLayoutDefinition {
    textComposer: PinyinTextComposer {
        pinyinScheme: "zrm"
        pinyinSchemeLocked: true
    }

    symbols: Shared.Symbols {}
    numbers: Shared.Numbers {}
    digits: Shared.Digits {}
    dialpad: Shared.Dialpad {}

    Shared.PhonePinyin {}
}
