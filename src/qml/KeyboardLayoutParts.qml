/*
 *  SPDX-FileCopyrightText: 2025 Felipe Martins Choi <fmcro2005@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */
import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.plasma.keyboard


// This element expects a KeyboardRow as a parent.
// The aspect ratio of the whole keyboard area (width/height) must be provided for it to scale properly.
Item {
    component LineGap : Item {
        property real aspectRatio: 0
        Layout.minimumWidth: 0
        Layout.preferredWidth: {
            if (aspectRatio > 2 && PlasmaKeyboardSettings.splitKeyboardEnabled){
                var factor = aspectRatio - 2
                return (12 * (factor **2) + parent.children.length * 10 *factor)
            }
            else{
                return 0
            }
        }
    }
}
