/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kcmutils as KCM

KCM.SimpleKCM {
    id: root

    Kirigami.FormLayout {
        id: formLayout

        width: flickable.width

        QQC2.Button {
            Kirigami.FormData.label: i18n("Keyboard layouts:")
            text: i18n("Configure…")
            icon.name: "languages"
            onClicked: kcm.push("KeyboardLayoutsPage.qml")
        }

        QQC2.CheckBox {
            id: soundsEnabled
            Kirigami.FormData.label: i18n("Key press feedback:")
            text: i18n("Sound")

            checked: kcm.plasmaKeyboardSettings.soundEnabled
            onCheckedChanged: kcm.plasmaKeyboardSettings.soundEnabled = checked
        }

        QQC2.CheckBox {
            id: vibrationEnabled
            text: i18n("Vibration")

            checked: kcm.plasmaKeyboardSettings.vibrationEnabled
            onCheckedChanged: kcm.plasmaKeyboardSettings.vibrationEnabled = checked
        }

        QQC2.CheckBox {
            id: characterPopupEnabled
            text: i18n("Character popup")

            checked: kcm.plasmaKeyboardSettings.characterPopupEnabled
            onCheckedChanged: kcm.plasmaKeyboardSettings.characterPopupEnabled = checked
        }

        QQC2.CheckBox {
            id: keyboardNavigationEnabled
            Kirigami.FormData.label: i18n("General:")
            text: i18n("Keyboard navigation")

            checked: kcm.plasmaKeyboardSettings.keyboardNavigationEnabled
            onCheckedChanged: kcm.plasmaKeyboardSettings.keyboardNavigationEnabled = checked
        }

        QQC2.CheckBox {
            id: autoCapitalizationEnabled
            text: i18n("Auto-capitalization")

            checked: kcm.plasmaKeyboardSettings.autoCapitalizationEnabled
            onCheckedChanged: kcm.plasmaKeyboardSettings.autoCapitalizationEnabled = checked
        }

        QQC2.CheckBox {
            id: diacriticsCheckbox
            Kirigami.FormData.label: i18n("Alternate characters:")
            text: i18n("Show popup when holding a key")

            checked: kcm.plasmaKeyboardSettings.diacriticsPopupEnabled
            onCheckedChanged: kcm.plasmaKeyboardSettings.diacriticsPopupEnabled = checked
        }

        QQC2.SpinBox {
            id: diacriticsDelaySpinBox
            Kirigami.FormData.label: i18n("Hold delay:")
            from: 100
            to: 1500
            stepSize: 50

            enabled: diacriticsCheckbox.checked
            value: kcm.plasmaKeyboardSettings.diacriticsHoldThresholdMs

            // Include the `milliseconds` suffix in the spinbox instead of the label
            textFromValue: function (value) {
                return value + " " + i18n("milliseconds");
            }

            // Parse the integer value from the spinbox text, ignoring the suffix
            valueFromText: function (text) {
                let number = parseInt(text);
                if (isNaN(number)) {
                    return kcm.plasmaKeyboardSettings.diacriticsHoldThresholdMs; // Fallback to current value if parsing fails
                }
                return number;
            }

            onValueChanged: kcm.plasmaKeyboardSettings.diacriticsHoldThresholdMs = value
        }

        QQC2.ComboBox {
            id: pinyinSchemeComboBox
            Kirigami.FormData.label: i18n("Chinese pinyin input:")

            textRole: "text"
            valueRole: "value"
            model: [
                { value: "full", text: i18n("Full pinyin") },
                { value: "zrm", text: i18n("Double pinyin (Ziranma)") },
                { value: "ms", text: i18n("Double pinyin (Microsoft)") },
                { value: "ziguang", text: i18n("Double pinyin (Ziguang)") },
                { value: "abc", text: i18n("Double pinyin (Smart ABC)") },
                { value: "pyjj", text: i18n("Double pinyin (Pinyin Plus Plus)") },
                { value: "xhe", text: i18n("Double pinyin (Xiaohe)") }
            ]

            currentIndex: Math.max(0, indexOfValue(kcm.plasmaKeyboardSettings.pinyinScheme))
            onActivated: kcm.plasmaKeyboardSettings.pinyinScheme = currentValue
        }
    }
}
