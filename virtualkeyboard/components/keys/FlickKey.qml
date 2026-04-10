// SPDX-FileCopyrightText: 2021 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick

import org.kde.plasma.keyboard.virtualkeyboard

// Key for 12-key layouts that supports directional flicks and repeated taps.

AbstractKey {
    id: root

    /*!
        Primary label shown on the key when no directional flick is active.
    */
    property string baseText: text

    /*!
        Text sent to the input method when the key is tapped without flicking.
    */
    property string baseCommitText: baseText

    /*!
        Commit texts for the directional alternatives.

        The order matches the visual alternative order after removing the base
        text from \l alternativeKeys.
    */
    property var alternativeCommitTexts: []

    /*!
        Enables repeated-tap cycling for 12-key style layouts.

        When enabled, repeated center taps ask the input method to replace the
        previously entered chunk with the next entry from the tap sequence.
    */
    property bool tapSequenceEnabled: false

    /*!
        Press position used to measure flick direction.
    */
    property point pressPoint: Qt.point(0, 0)

    /*!
        Currently highlighted label while the pointer is down.
    */
    property string currentText: baseText

    /*!
        Commit text paired with \l currentText.
    */
    property string currentCommitText: baseCommitText

    /*!
        Internal index for the active repeated-tap sequence.
    */
    property int tapSequenceIndex: -1

    /*!
        Distance from the press point that still counts as a center tap.
    */
    readonly property real centerRadius: width * 0.4

    /*!
        Directional alternatives with the base entry removed.
    */
    readonly property var flickKeys: {
        const keys = typeof alternativeKeys === "string" ? alternativeKeys.split("") : (alternativeKeys || [])
        const baseIndex = keys.indexOf(baseText)
        if (baseIndex === -1) {
            return keys
        }
        return keys.slice(0, baseIndex).concat(keys.slice(baseIndex + 1))
    }
    readonly property string flickLeft: flickKeys.length > 0 ? flickKeys[0] : ""
    readonly property string flickTop: flickKeys.length > 2 ? flickKeys[1] : ""
    readonly property string flickBottom: flickKeys.length > 3 ? flickKeys[3] : (flickKeys.length > 2 ? flickKeys[2] : "")
    readonly property string flickRight: flickKeys.length > 3 ? flickKeys[2] : (flickKeys.length === 2 ? flickKeys[1] : "")
    readonly property string flickCommitLeft: alternativeCommitTexts.length > 0 ? alternativeCommitTexts[0] : flickLeft
    readonly property string flickCommitTop: alternativeCommitTexts.length > 2 ? alternativeCommitTexts[1] : flickTop
    readonly property string flickCommitBottom: alternativeCommitTexts.length > 3 ? alternativeCommitTexts[3] : (alternativeCommitTexts.length > 2 ? alternativeCommitTexts[2] : flickBottom)
    readonly property string flickCommitRight: alternativeCommitTexts.length > 3 ? alternativeCommitTexts[2] : (alternativeCommitTexts.length === 2 ? alternativeCommitTexts[1] : flickRight)

    /*!
        Ordered commit texts used for repeated center taps.
    */
    readonly property var tapSequenceCommitTexts: [baseCommitText].concat(alternativeCommitTexts)

    displayText: pressedVisual ? currentText : baseText

    function clearTapSequence() {
        tapSequenceIndex = -1
        tapSequenceTimer.stop()
    }

    function angle(point) {
        const dx = point.x - pressPoint.x
        const dy = point.y - pressPoint.y
        const theta = Math.atan2(-dy, dx) * 360 / (2 * Math.PI)
        return theta < 0 ? theta + 360 : theta
    }

    function distance(point) {
        const dx = point.x - pressPoint.x
        const dy = point.y - pressPoint.y
        return Math.sqrt(dx * dx + dy * dy)
    }

    function keyForText(textValue) {
        return textValue && textValue.length === 1 ? textValue.charCodeAt(0) : Qt.Key_unknown
    }

    function commitTapSequence() {
        if (!tapSequenceEnabled || tapSequenceCommitTexts.length === 0) {
            return false
        }

        let nextIndex = 0
        if (tapSequenceTimer.running) {
            nextIndex = (tapSequenceIndex + 1) % tapSequenceCommitTexts.length
            if (tapSequenceIndex >= 0 && VirtualKeyboard.inputEngine && VirtualKeyboard.inputEngine.inputMethod.replaceLastInput(tapSequenceCommitTexts[nextIndex])) {
                tapSequenceIndex = nextIndex
                tapSequenceTimer.restart()
                return true
            }
        }

        tapSequenceIndex = 0
        tapSequenceTimer.restart()
        return false
    }

    function trigger() {
        if (!enabled) {
            return;
        }
        if (VirtualKeyboard.inputEngine) {
            VirtualKeyboard.inputEngine.sendInputMethodKey(root.keyForText(root.baseText), root.baseCommitText);
        }
        clicked();
    }

    function updateCurrentText(x, y) {
        const point = Qt.point(x, y)
        if (distance(point) < centerRadius) {
            currentText = baseText
            currentCommitText = baseCommitText
            return
        }

        const currentAngle = angle(point)
        if (currentAngle < 45 || currentAngle > 315) {
            currentText = flickRight || baseText
            currentCommitText = flickCommitRight || currentText
        } else if (currentAngle < 135) {
            currentText = flickTop || baseText
            currentCommitText = flickCommitTop || currentText
        } else if (currentAngle < 225) {
            currentText = flickLeft || baseText
            currentCommitText = flickCommitLeft || currentText
        } else {
            currentText = flickBottom || baseText
            currentCommitText = flickCommitBottom || currentText
        }
    }

    Timer {
        id: tapSequenceTimer
        interval: 1200
        repeat: false
        onTriggered: root.clearTapSequence()
    }

    KeyMouseArea {
        keyItem: root

        onPressStarted: (point) => {
            root.pressPoint = Qt.point(point.x, point.y)
            root.currentText = root.baseText
            root.currentCommitText = root.baseCommitText
            const previewPopup = VirtualKeyboard.flickPreviewPopup
            if (previewPopup) {
                previewPopup.openForKey(root)
            }
        }

        onPositionChanged: (point) => {
            root.updateCurrentText(point.x, point.y)
            const previewPopup = VirtualKeyboard.flickPreviewPopup
            if (previewPopup) {
                previewPopup.updateFromKey(root)
            }
        }

        onReleaseFinished: {
            const previewPopup = VirtualKeyboard.flickPreviewPopup
            if (previewPopup) {
                previewPopup.close()
            }
            const usedExplicitFlick = root.currentText !== root.baseText || root.currentCommitText !== root.baseCommitText
            if (usedExplicitFlick) {
                root.clearTapSequence()
            } else if (root.commitTapSequence()) {
                root.clicked()
                root.currentText = root.baseText
                root.currentCommitText = root.baseCommitText
                return
            }

            if (VirtualKeyboard.inputEngine) {
                VirtualKeyboard.inputEngine.sendInputMethodKey(root.keyForText(root.currentText), root.currentCommitText)
            }
            root.clicked()
            root.currentText = root.baseText
            root.currentCommitText = root.baseCommitText
        }

        onCancelFinished: {
            const previewPopup = VirtualKeyboard.flickPreviewPopup
            if (previewPopup) {
                previewPopup.close()
            }
            root.clearTapSequence()
            root.currentText = root.baseText
            root.currentCommitText = root.baseCommitText
        }
    }
}
