// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick

MouseArea {
    id: root

    /*!
        Key item that owns the pressed visual state and feedback behavior.
    */
    required property Item keyItem

    /*!
        Whether holding this key should repeatedly trigger \l repeatTriggered.
    */
    property bool repeatEnabled: keyItem.repeat

    /*!
        Delay before key repeat starts.
    */
    property int repeatDelay: 600

    /*!
        Interval used after key repeat starts.
    */
    property int repeatInterval: 50

    signal pressStarted(var mouse)
    signal releaseFinished(var mouse)
    signal cancelFinished()
    signal repeatTriggered()

    anchors.fill: parent
    z: 1
    acceptedButtons: Qt.LeftButton

    function stopRepeat() {
        repeatDelayTimer.stop();
        repeatTimer.stop();
    }

    Timer {
        id: repeatDelayTimer
        interval: root.repeatDelay
        repeat: false
        onTriggered: repeatTimer.start()
    }

    Timer {
        id: repeatTimer
        interval: root.repeatInterval
        repeat: true
        onTriggered: root.repeatTriggered()
    }

    onPressed: (mouse) => {
        keyItem.pressedVisual = true;
        keyItem.playPressFeedback();
        root.pressStarted(mouse);
        if (root.repeatEnabled) {
            root.repeatTriggered();
            repeatDelayTimer.start();
        }
    }

    onReleased: (mouse) => {
        keyItem.pressedVisual = false;
        root.stopRepeat();
        root.releaseFinished(mouse);
    }

    onCanceled: {
        keyItem.pressedVisual = false;
        root.stopRepeat();
        root.cancelFinished();
    }
}
