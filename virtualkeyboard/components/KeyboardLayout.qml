// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.keyboard.virtualkeyboard

ColumnLayout {
    id: root

    // Default weight of keys
    property real defaultKeyWeight: 100

    // Default height weight of keyboard rows
    property real defaultRowHeightWeight: 100

    // Set a maximum width of the keyboard in relation to its height (so it doesn't stretch forever)
    property real maxWidthToHeightRatio: -1

    // Multiplier for the keyboard panel height. Use this for layouts with extra rows.
    property real panelHeightFactor: 1

    property string keyboardLayoutId: ""
    property string packageId: ""
    property var virtualKeyboardContext: null
    readonly property var inputEngine: virtualKeyboardContext ? virtualKeyboardContext.inputEngine : VirtualKeyboard.inputEngine
    property var inputMethod
    property var sharedLayouts: []

    Layout.fillWidth: true
    Layout.fillHeight: true
    width: parent ? parent.width : implicitWidth
    height: parent ? parent.height : implicitHeight
    spacing: 0

    function applyEngineBindings() {
        let effectiveInputMethod = inputMethod
        if (!effectiveInputMethod) {
            effectiveInputMethod = createInputMethod()
            if (effectiveInputMethod) {
                inputMethod = effectiveInputMethod
            }
        }
        if (effectiveInputMethod && inputEngine) {
            inputEngine.inputMethod = effectiveInputMethod
        }
    }

    Component.onCompleted: applyEngineBindings()
    onVirtualKeyboardContextChanged: if (visible) applyEngineBindings()
    onVisibleChanged: if (visible) applyEngineBindings()
    onKeyboardLayoutIdChanged: if (visible) applyEngineBindings()
    onPackageIdChanged: if (visible) applyEngineBindings()
    onInputMethodChanged: if (visible) applyEngineBindings()
    function createInputMethod() {
        return defaultInputMethodComponent.createObject(root)
    }

    Component {
        id: defaultInputMethodComponent

        DirectInputMethod {}
    }
}
