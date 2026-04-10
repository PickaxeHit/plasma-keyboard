/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

ListView {
    id: root

    property var sourceModel: []
    property string searchText: ""

    model: sourceModel

    function updateModel() {
        const filteredLayouts = []
        for (const layout of sourceModel) {
            const name = (layout.name || "").toLowerCase()
            const description = (layout.description || "").toLowerCase()
            const filter = searchText.toLowerCase()
            if (filter.length === 0 || name.indexOf(filter) !== -1 || description.indexOf(filter) !== -1) {
                filteredLayouts.push(layout)
            }
        }
        model = filteredLayouts
    }

    onSearchTextChanged: updateModel()
    onSourceModelChanged: updateModel()
    Component.onCompleted: {
        sourceModel = kcm.availableKeyboardLayouts
        updateModel()
    }

    headerPositioning: ListView.OverlayHeader
    header: QQC2.ToolBar {
        width: parent.width
        z: 999
        position: QQC2.ToolBar.Header

        topPadding: Kirigami.Units.largeSpacing
        bottomPadding: Kirigami.Units.largeSpacing
        leftPadding: Kirigami.Units.largeSpacing
        rightPadding: Kirigami.Units.largeSpacing

        Kirigami.Theme.inherit: false
        Kirigami.Theme.colorSet: Kirigami.Theme.Window

        contentItem: ColumnLayout {
            spacing: 0

            Kirigami.SearchField {
                id: searchField
                placeholderText: i18n("Filter keyboard layouts…")
                Accessible.name: i18n("Filter keyboard layouts")

                Layout.fillWidth: true

                onTextChanged: {
                    root.searchText = text
                    searchField.forceActiveFocus()
                }
            }

            Kirigami.InlineMessage {
                Layout.topMargin: Kirigami.Units.smallSpacing
                Layout.fillWidth: true
                text: i18n("No keyboard layouts selected. The default keyboard layout for the system will be used.")
                type: Kirigami.MessageType.Information
                visible: kcm.enabledKeyboardLayoutIds.length === 0
            }
        }
    }

    delegate: QQC2.CheckDelegate {
        required property var modelData

        width: root.width
        text: modelData.name
        checked: kcm.enabledKeyboardLayoutIds.includes(modelData.id)

        contentItem: ColumnLayout {
            spacing: Kirigami.Units.smallSpacing

            QQC2.Label {
                Layout.fillWidth: true
                text: modelData.name
                elide: Text.ElideRight
            }

            QQC2.Label {
                Layout.fillWidth: true
                text: modelData.description
                visible: text.length > 0
                elide: Text.ElideRight
                opacity: 0.7
                font: Kirigami.Theme.smallFont
            }
        }

        onCheckedChanged: {
            if (checked) {
                kcm.enableKeyboardLayout(modelData.id)
            } else {
                kcm.disableKeyboardLayout(modelData.id)
            }
        }
    }
}
