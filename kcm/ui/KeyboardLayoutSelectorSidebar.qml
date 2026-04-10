/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.bigscreen as Bigscreen

Bigscreen.SidebarOverlay {
    id: root

    openFocusItem: layouts
    header: Bigscreen.SidebarOverlayHeader {
        title: i18n("Keyboard Layouts")
    }

    content: QQC2.ScrollView {
        ListView {
            id: layouts
            Layout.fillWidth: true
            implicitHeight: contentHeight
            clip: true
            model: kcm.availableKeyboardLayouts

            delegate: Bigscreen.SwitchDelegate {
                required property var modelData

                width: layouts.width
                text: modelData.name
                onClicked: checked = !checked
                Keys.onReturnPressed: checked = !checked
                checked: kcm.enabledKeyboardLayoutIds.includes(modelData.id)

                onCheckedChanged: {
                    if (checked) {
                        kcm.enableKeyboardLayout(modelData.id)
                    } else {
                        kcm.disableKeyboardLayout(modelData.id)
                    }
                }
            }
        }
    }
}
