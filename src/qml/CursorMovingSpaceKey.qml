/*
 *  SPDX-FileCopyrightText: 2025 Tadeáš Erban
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick
import QtQuick.VirtualKeyboard

SpaceKey {
    id: root;

    property point origin;

    // Allows for Keyboard.qml to send us the touch position
    keyType: QtVirtualKeyboard.KeyType.FlickKey

    function press(x, y) {
        origin = Qt.point(x, y);
        text = " ";
        key = text.toUpperCase().charCodeAt(0)
    }

    function update(x, y) {
        var change = x - origin.x;
        if (Math.abs(change) > 10) {
            origin = Qt.point(x, y);
            Vibration.vibrate(10);
            text = "";
            key = 0;

            // Move cursor
            InputContext.inputEngine.virtualKeyClick((change < 0) ? Qt.Key_Left : Qt.Key_Right, "", 0);
        }
    }
}
