/*
 *  SPDX-FileCopyrightText: 2025 Felipe Martins Choi <fmcro2005@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */
import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.plasma.keyboard


// This element expects a KeyboardRow as a parent
// and expects the parent's parent to be an item with explicit width and height,
// with id "inputPanel". PlasmaKeyboard's root element and the per-locale layouts provide such preconditions.
// LineGaps are horizontal empty spaces that attempt to occupy as much space as needed
// between the left half and the right half of a split keyboard
Item {
    component LineGap : Item {
        property real aspectRatio: inputPanel.width / inputPanel.height

        //weights for a second degree polynomial growth function.
        property real linearGrowth: 10 * parent.children.length
        property real quadraticGrowth: 12

        // The smallest value of width-to-height ratio at which the gap has size
        // greater than 0
        property real cutoff: 2
        Layout.minimumWidth: 0
        Layout.preferredWidth: {
            if (aspectRatio > 2 && PlasmaKeyboardSettings.splitKeyboardEnabled){
                var factor = aspectRatio - cutoff
                return (quadraticGrowth * (factor **2) + linearGrowth * factor)
            }
            else{
                return 0
            }
        }
    }
}
