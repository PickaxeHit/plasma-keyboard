// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.keyboard.virtualkeyboard
import org.kde.plasma.keyboard.virtualkeyboard.components

KeyboardLayout {
    id: root

    readonly property int rowCount: 4
    readonly property int desktopRowWeight: 1300

    // Keep normal key slots square: 13 key-width units across / 4 key-height units tall.
    maxWidthToHeightRatio: (desktopRowWeight / defaultKeyWeight) / rowCount

    KeyboardRow {
        KeysymKey { key: Qt.Key_Escape; displayText: "Esc"; }
        Key { key: Qt.Key_Q; text: "q"; alternativeKeys: "q1"; smallText: "1"}
        Key { key: Qt.Key_W; text: "w"; alternativeKeys: "w2"; smallText: "2"}
        Key { key: Qt.Key_E; text: "e"; alternativeKeys: "êe3ëèé"; smallText: "3"}
        Key { key: Qt.Key_R; text: "r"; alternativeKeys: "ŕr4ř"; smallText: "4"}
        Key { key: Qt.Key_T; text: "t"; alternativeKeys: "ţt5ŧť"; smallText: "5"}
        Key { key: Qt.Key_Y; text: "y"; alternativeKeys: "ÿy6ýŷ"; smallText: "6"}
        Key { key: Qt.Key_U; text: "u"; alternativeKeys: "űūũûüu7ùú"; smallText: "7"}
        Key { key: Qt.Key_I; text: "i"; alternativeKeys: "îïīĩi8ìí"; smallText: "8"}
        Key { key: Qt.Key_O; text: "o"; alternativeKeys: "œøõôöòóo9"; smallText: "9"}
        Key { key: Qt.Key_P; text: "p"; alternativeKeys: "p0"; smallText: "0"}
        BackspaceKey { weight: root.defaultKeyWeight * 2 }
    }

    KeyboardRow {
        KeysymKey { weight: defaultKeyWeight * 1.5; key: Qt.Key_Tab; displayText: "Tab" }
        Key { key: Qt.Key_A; text: "a"; alternativeKeys: "aäåãâàá" }
        Key { key: Qt.Key_S; text: "s"; alternativeKeys: "šsşś" }
        Key { key: Qt.Key_D; text: "d"; alternativeKeys: "dđď" }
        Key { key: Qt.Key_F; text: "f" }
        Key { key: Qt.Key_G; text: "g"; alternativeKeys: "ġgģĝğ" }
        Key { key: Qt.Key_H; text: "h" }
        Key { key: Qt.Key_J; text: "j" }
        Key { key: Qt.Key_K; text: "k" }
        Key { key: Qt.Key_L; text: "l"; alternativeKeys: "ĺŀłļľl" }
        Key { key: Qt.Key_Apostrophe; text: '\''; alternativeKeys: "\""; smallText: "\""}
        EnterKey { weight: root.defaultKeyWeight * 1.5 }
    }

    KeyboardRow {
        ShiftKey { weight: root.defaultKeyWeight * 2; sendDirectKeyEvents: true }
        Key { key: Qt.Key_Z; text: "z"; alternativeKeys: "zžż" }
        Key { key: Qt.Key_X; text: "x" }
        Key { key: Qt.Key_C; text: "c"; alternativeKeys: "çcċčć" }
        Key { key: Qt.Key_V; text: "v" }
        Key { key: Qt.Key_B; text: "b" }
        Key { key: Qt.Key_N; text: "n"; alternativeKeys: "ņńnň" }
        Key { key: Qt.Key_M; text: "m" }
        Key { key: Qt.Key_Comma; text: ","; alternativeKeys: ";"; smallText: ";"}
        Key { key: Qt.Key_Period; text: "."; alternativeKeys: ":"; smallText: ":"}
        Key { key: Qt.Key_Question; text: "?"; alternativeKeys: "!"; smallText: "!"}
        ShiftKey { sendDirectKeyEvents: true }
    }

    KeyboardRow {
        SymbolModeKey { }
        KeysymKey { key: Qt.Key_Control; displayText: "Ctrl"; sticky: true }
        KeysymKey { key: Qt.Key_Meta; iconName: "start-here-symbolic"; displayText: ""; sticky: true }
        KeysymKey { key: Qt.Key_Alt; displayText: "Alt"; sticky: true }
        SpaceKey { weight: root.defaultKeyWeight * 5 }
        KeysymKey { key: Qt.Key_Left; iconName: "go-previous-symbolic"; displayText: ""; repeat: true }
        KeyboardColumn {
            weight: root.defaultKeyWeight
            KeysymKey { implicitHeight: -1; key: Qt.Key_Up; iconName: "go-up-symbolic"; displayText: ""; repeat: true }
            KeysymKey { implicitHeight: -1; key: Qt.Key_Down; iconName: "go-down-symbolic"; displayText: ""; repeat: true }
        }
        KeysymKey { key: Qt.Key_Right; iconName: "go-next-symbolic"; displayText: ""; repeat: true }
        ChangeLanguageKey { weight: root.defaultKeyWeight / 2 }
        HideKeyboardKey { weight: root.defaultKeyWeight / 2 }
    }
}
