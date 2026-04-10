// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.keyboard
import org.kde.plasma.keyboard.virtualkeyboard
import org.kde.plasma.keyboard.virtualkeyboard.components

KeyboardLayout {
    id: root

    readonly property int rowCount: 5
    readonly property int desktopRowWeight: 1550
    readonly property bool fnActive: inputEngine && inputEngine.pressedKeys.indexOf(KeyCodes.KeyFn) !== -1

    panelHeightFactor: 1.1875

    // Keep normal key slots square: 15.5 key-width units across / 5 key-height units tall.
    maxWidthToHeightRatio: (desktopRowWeight / defaultKeyWeight) / rowCount

    KeyboardRow {
        heightWeight: defaultRowHeightWeight * 0.75

        KeysymKey { key: Qt.Key_Escape; displayText: "Esc" }
        Key { key: Qt.Key_QuoteLeft; text: "`"; alternativeKeys: "`~"; smallText: "~" }
        Key {
            key: root.fnActive ? Qt.Key_F1 : Qt.Key_1
            text: root.fnActive ? "" : "1"
            displayText: root.fnActive ? "F1" : "1"
            alternativeKeys: root.fnActive ? [] : "1!"
            smallText: root.fnActive ? "" : "!"
        }
        Key {
            key: root.fnActive ? Qt.Key_F2 : Qt.Key_2
            text: root.fnActive ? "" : "2"
            displayText: root.fnActive ? "F2" : "2"
            alternativeKeys: root.fnActive ? [] : "2@"
            smallText: root.fnActive ? "" : "@"
        }
        Key {
            key: root.fnActive ? Qt.Key_F3 : Qt.Key_3
            text: root.fnActive ? "" : "3"
            displayText: root.fnActive ? "F3" : "3"
            alternativeKeys: root.fnActive ? [] : "3#"
            smallText: root.fnActive ? "" : "#"
        }
        Key {
            key: root.fnActive ? Qt.Key_F4 : Qt.Key_4
            text: root.fnActive ? "" : "4"
            displayText: root.fnActive ? "F4" : "4"
            alternativeKeys: root.fnActive ? [] : "4$"
            smallText: root.fnActive ? "" : "$"
        }
        Key {
            key: root.fnActive ? Qt.Key_F5 : Qt.Key_5
            text: root.fnActive ? "" : "5"
            displayText: root.fnActive ? "F5" : "5"
            alternativeKeys: root.fnActive ? [] : "5%"
            smallText: root.fnActive ? "" : "%"
        }
        Key {
            key: root.fnActive ? Qt.Key_F6 : Qt.Key_6
            text: root.fnActive ? "" : "6"
            displayText: root.fnActive ? "F6" : "6"
            alternativeKeys: root.fnActive ? [] : "6^"
            smallText: root.fnActive ? "" : "^"
        }
        Key {
            key: root.fnActive ? Qt.Key_F7 : Qt.Key_7
            text: root.fnActive ? "" : "7"
            displayText: root.fnActive ? "F7" : "7"
            alternativeKeys: root.fnActive ? [] : "7&"
            smallText: root.fnActive ? "" : "&"
        }
        Key {
            key: root.fnActive ? Qt.Key_F8 : Qt.Key_8
            text: root.fnActive ? "" : "8"
            displayText: root.fnActive ? "F8" : "8"
            alternativeKeys: root.fnActive ? [] : "8*"
            smallText: root.fnActive ? "" : "*"
        }
        Key {
            key: root.fnActive ? Qt.Key_F9 : Qt.Key_9
            text: root.fnActive ? "" : "9"
            displayText: root.fnActive ? "F9" : "9"
            alternativeKeys: root.fnActive ? [] : "9("
            smallText: root.fnActive ? "" : "("
        }
        Key {
            key: root.fnActive ? Qt.Key_F10 : Qt.Key_0
            text: root.fnActive ? "" : "0"
            displayText: root.fnActive ? "F10" : "0"
            alternativeKeys: root.fnActive ? [] : "0)"
            smallText: root.fnActive ? "" : ")"
        }
        Key {
            key: root.fnActive ? Qt.Key_F11 : Qt.Key_Minus
            text: root.fnActive ? "" : "-"
            displayText: root.fnActive ? "F11" : "-"
            alternativeKeys: root.fnActive ? [] : "-_"
            smallText: root.fnActive ? "" : "_"
        }
        Key {
            key: root.fnActive ? Qt.Key_F12 : Qt.Key_Equal
            text: root.fnActive ? "" : "="
            displayText: root.fnActive ? "F12" : "="
            alternativeKeys: root.fnActive ? [] : "=+"
            smallText: root.fnActive ? "" : "+"
        }
        BackspaceKey { weight: root.defaultKeyWeight * 1.5 }
    }

    KeyboardRow {
        KeysymKey { key: Qt.Key_Tab; displayText: "Tab"; weight: root.defaultKeyWeight * 1.5 }
        Key { key: Qt.Key_Q; text: "q" }
        Key { key: Qt.Key_W; text: "w" }
        Key { key: Qt.Key_E; text: "e" }
        Key { key: Qt.Key_R; text: "r" }
        Key { key: Qt.Key_T; text: "t" }
        Key { key: Qt.Key_Y; text: "y" }
        Key { key: Qt.Key_U; text: "u" }
        Key { key: Qt.Key_I; text: "i" }
        Key { key: Qt.Key_O; text: "o" }
        Key { key: Qt.Key_P; text: "p" }
        Key { key: Qt.Key_BracketLeft; text: "["; alternativeKeys: "[{"; smallText: "{" }
        Key { key: Qt.Key_BracketRight; text: "]"; alternativeKeys: "]}"; smallText: "}" }
        Key { key: Qt.Key_Backslash; text: "\\"; alternativeKeys: "\\|"; smallText: "|" }
        KeysymKey { key: Qt.Key_Delete; displayText: "Del" }
    }

    KeyboardRow {
        KeysymKey {
            key: Qt.Key_CapsLock
            displayText: "Caps"
            weight: root.defaultKeyWeight * 1.75
            highlighted: inputEngine && inputEngine.capsLockActive
            onClicked: {
                if (!inputEngine) {
                    return;
                }
                inputEngine.capsLockActive = !inputEngine.capsLockActive;
                inputEngine.shiftActive = false;
            }
        }
        Key { key: Qt.Key_A; text: "a" }
        Key { key: Qt.Key_S; text: "s" }
        Key { key: Qt.Key_D; text: "d" }
        Key { key: Qt.Key_F; text: "f" }
        Key { key: Qt.Key_G; text: "g" }
        Key { key: Qt.Key_H; text: "h" }
        Key { key: Qt.Key_J; text: "j" }
        Key { key: Qt.Key_K; text: "k" }
        Key { key: Qt.Key_L; text: "l" }
        Key { key: Qt.Key_Semicolon; text: ";"; alternativeKeys: ";:"; smallText: ":" }
        Key { key: Qt.Key_Apostrophe; text: "'"; alternativeKeys: "'\""; smallText: "\"" }
        EnterKey { weight: root.defaultKeyWeight * 2.75 }
    }

    KeyboardRow {
        ShiftKey { weight: root.defaultKeyWeight * 2.5; sendDirectKeyEvents: true }
        Key { key: Qt.Key_Z; text: "z" }
        Key { key: Qt.Key_X; text: "x" }
        Key { key: Qt.Key_C; text: "c" }
        Key { key: Qt.Key_V; text: "v" }
        Key { key: Qt.Key_B; text: "b" }
        Key { key: Qt.Key_N; text: "n" }
        Key { key: Qt.Key_M; text: "m" }
        Key { key: Qt.Key_Comma; text: ","; alternativeKeys: ",<"; smallText: "<" }
        Key { key: Qt.Key_Period; text: "."; alternativeKeys: ".>"; smallText: ">" }
        Key { key: Qt.Key_Slash; text: "/"; alternativeKeys: "/?"; smallText: "?" }
        KeysymKey { key: Qt.Key_Up; iconName: "go-up-symbolic"; displayText: ""; repeat: true }
        ShiftKey { weight: root.defaultKeyWeight * 2; sendDirectKeyEvents: true }
    }

    KeyboardRow {
        KeysymKey { key: Qt.Key_Control; displayText: "Ctrl"; sticky: true }
        KeysymKey { key: KeyCodes.KeyFn; displayText: "Fn"; sticky: true }
        KeysymKey { key: Qt.Key_Meta; iconName: "start-here-symbolic"; displayText: ""; sticky: true }
        KeysymKey { key: Qt.Key_Alt; displayText: "Alt"; sticky: true }
        SpaceKey { weight: root.defaultKeyWeight * 7.5 }
        KeysymKey { key: Qt.Key_Left; iconName: "go-previous-symbolic"; displayText: ""; repeat: true }
        KeysymKey { key: Qt.Key_Down; iconName: "go-down-symbolic"; displayText: ""; repeat: true }
        KeysymKey { key: Qt.Key_Right; iconName: "go-next-symbolic"; displayText: ""; repeat: true }
        ChangeLanguageKey { weight: root.defaultKeyWeight / 2 }
        HideKeyboardKey { weight: root.defaultKeyWeight / 2 }
    }
}
