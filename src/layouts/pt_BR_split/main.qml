// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick
import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Components
import QtQuick.Layouts



KeyboardLayout {
    inputMode: InputEngine.InputMode.Latin
    keyWeight: 160
    readonly property real normalKeyWidth: normalKey.width
    readonly property real functionKeyWidth: mapFromItem(normalKey, normalKey.width / 2, 0).x
    KeyboardRow {
        Layout.maximumHeight:52
        Item{
            Layout.maximumWidth:16
            Layout.minimumWidth:16
        }
        Key {
            Layout.maximumWidth: 52
            key: Qt.Key_Q
            text: "q"
        }
        Key {
            Layout.maximumWidth: 52
            id: normalKey
            key: Qt.Key_W
            text: "w"
        }
        Key {
            Layout.maximumWidth: 52
            key: Qt.Key_E
            text: "e"
            alternativeKeys: "ēęėëeêèé"
        }
        Key {
            Layout.maximumWidth: 52
            key: Qt.Key_R
            text: "r"
        }
        Item{
            Layout.minimumWidth: 16
            Layout.fillWidth: true
        }
        Key {
            Layout.maximumWidth: 52
            key: Qt.Key_T
            text: "t"
        }
        Key {
            Layout.maximumWidth: 52
            key: Qt.Key_Y
            text: "y"
        }
        Key {
            Layout.maximumWidth: 52
            key: Qt.Key_U
            text: "u"
            alternativeKeys: "üûuùú"
        }
        Key {
            Layout.maximumWidth: 52
            key: Qt.Key_I
            text: "i"
            alternativeKeys: "ïįîiìí"
        }
        Key {
            Layout.maximumWidth: 52
            key: Qt.Key_O
            text: "o"
            alternativeKeys: "öõôoòó"
        }
        Key {
            Layout.maximumWidth: 52
            key: Qt.Key_P
            text: "p"
        }
        Item{
            Layout.maximumWidth:16
            Layout.minimumWidth:16
        }
    }
    KeyboardRow {
        Layout.maximumHeight:52
        Key {
            Layout.maximumWidth: 52
            key: Qt.Key_A
            text: "a"
            alternativeKeys: "äãaâàá"
        }
        Key {
            Layout.maximumWidth: 52
            key: Qt.Key_S
            text: "s"
        }
        Key {
            Layout.maximumWidth: 52
            key: Qt.Key_D
            text: "d"
        }
        Key {
            Layout.maximumWidth: 52
            key: Qt.Key_F
            text: "f"
        }
        Key {
            Layout.maximumWidth: 52
            key: Qt.Key_G
            text: "g"
        }
        Item{
            Layout.minimumWidth: 16
            Layout.fillWidth: true
        }
        Key {
            Layout.maximumWidth: 52
            key: Qt.Key_H
            text: "h"
        }
        Key {
            Layout.maximumWidth: 52
            key: Qt.Key_J
            text: "j"
        }
        Key {
            Layout.maximumWidth: 52
            key: Qt.Key_K
            text: "k"
        }
        Key {
            Layout.maximumWidth: 52
            key: Qt.Key_L
            text: "l"
        }
        Key {
            Layout.maximumWidth: 52
            key: Qt.Key_Ccedilla
            text: "ç"
        }
    }
    KeyboardRow {
        Layout.maximumHeight:52
        ShiftKey {
            weight: functionKeyWidth
            Layout.fillWidth: false
        }
        Key {
            Layout.maximumWidth: 52
            key: Qt.Key_Z
            text: "z"
        }
        Key {
            Layout.maximumWidth: 52
            key: Qt.Key_X
            text: "x"
        }
        Key {
            Layout.maximumWidth: 52
            key: Qt.Key_C
            text: "c"
            alternativeKeys: "čcć"
        }
        Item{
            Layout.minimumWidth: 16
            Layout.fillWidth: true
        }
        Key {
            Layout.maximumWidth: 52
            key: Qt.Key_V
            text: "v"
        }
        Key {
            Layout.maximumWidth: 52
            key: Qt.Key_B
            text: "b"
        }
        Key {
            Layout.maximumWidth: 52
            key: Qt.Key_N
            text: "n"
        }
        Key {
            Layout.maximumWidth: 52
            key: Qt.Key_M
            text: "m"
        }
        BackspaceKey {
            weight: functionKeyWidth
            Layout.fillWidth: false
        }
    }
    KeyboardRow {
        Layout.maximumHeight:52
        SymbolModeKey {
            weight: functionKeyWidth
            Layout.fillWidth: false
        }
        ChangeLanguageKey {
            weight: normalKeyWidth
            Layout.fillWidth: false
        }
        Key {
            Layout.maximumWidth: 52
            key: Qt.Key_Comma
            weight: normalKeyWidth
            Layout.fillWidth: false
            text: ","
            smallText: "\u2699"
            smallTextVisible: keyboard.isFunctionPopupListAvailable()
            highlighted: true
        }
        SpaceKey {
        }
        Key {
            Layout.preferredWidth: 52
            key: Qt.Key_Period
            weight: normalKeyWidth
            Layout.fillWidth: false
            text: "."
            alternativeKeys: "!.?"
            smallText: "!?"
            smallTextVisible: true
            highlighted: true
        }
        HideKeyboardKey {
            Layout.maximumWidth: 52
            weight: normalKeyWidth
            Layout.fillWidth: false
        }
        EnterKey {
            weight: functionKeyWidth
            Layout.fillWidth: false
        }
    }
}
