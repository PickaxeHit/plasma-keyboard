// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick

import org.kde.plasma.keyboard.virtualkeyboard

// Action key that cycles through input modes provided by a keyboard layout.

ActionKey {
    id: root

    property var modes: []
    property var inputMethod

    function modeEntries() {
        const configuredModes = modes && modes.length > 0 ? modes : []
        return configuredModes.map(function(mode) {
            if (typeof mode === "string") {
                return { value: mode, label: mode }
            }
            return {
                value: mode.value ?? "",
                label: mode.label ?? mode.value ?? ""
            }
        }).filter(function(mode) {
            return mode.value.length > 0
        })
    }

    function nextInputMode() {
        const entries = modeEntries()
        if (!entries || entries.length === 0) {
            return null
        }
        const currentMode = inputMethod ? inputMethod.inputMode : ""
        const currentIndex = entries.findIndex(function(mode) {
            return mode.value === currentMode
        })
        if (currentIndex === -1 || currentIndex + 1 >= entries.length) {
            return entries[0]
        }
        return entries[currentIndex + 1]
    }

    displayText: {
        const nextMode = nextInputMode()
        return nextMode ? nextMode.label : "Mode"
    }
    functionKey: true
    secondaryStyle: true

    onClicked: {
        const nextMode = nextInputMode()
        if (inputMethod && nextMode) {
            inputMethod.inputMode = nextMode.value
        }
    }
}
