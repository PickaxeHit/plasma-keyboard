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

    property string searchText: ""
    property var expandedGroupIds: []

    model: kcm.availableKeyboardLayoutGroups

    function layoutMatches(layout, filter) {
        const name = (layout.name || "").toLowerCase();
        const description = (layout.description || "").toLowerCase();
        return filter.length === 0 || name.indexOf(filter) !== -1 || description.indexOf(filter) !== -1;
    }

    function groupMatches(groupName, groupDescription, layouts, filter) {
        if (groupNameMatches(groupName, groupDescription, filter)) {
            return visibleLayoutCount(layouts, "") > 0;
        }

        if (!layouts) {
            return false;
        }

        for (let i = 0; i < layouts.count; ++i) {
            if (layoutMatches(layouts.get(i), filter)) {
                return true;
            }
        }
        return false;
    }

    function groupNameMatches(groupName, groupDescription, filter) {
        const normalizedName = (groupName || "").toLowerCase();
        const normalizedDescription = (groupDescription || "").toLowerCase();
        return filter.length === 0 || normalizedName.indexOf(filter) !== -1 || normalizedDescription.indexOf(filter) !== -1;
    }

    function groupVisible(groupName, groupDescription, layouts) {
        const filter = searchText.toLowerCase();
        return groupMatches(groupName, groupDescription, layouts, filter);
    }

    function visibleLayoutCount(layouts, filter) {
        let count = 0;
        if (!layouts) {
            return count;
        }

        for (let i = 0; i < layouts.count; ++i) {
            if (layoutMatches(layouts.get(i), filter)) {
                ++count;
            }
        }
        return count;
    }

    function isExpanded(groupId) {
        return expandedGroupIds.includes(groupId);
    }

    function toggleExpanded(groupId) {
        const expanded = expandedGroupIds.slice();
        const index = expanded.indexOf(groupId);
        if (index === -1) {
            expanded.push(groupId);
        } else {
            expanded.splice(index, 1);
        }
        expandedGroupIds = expanded;
    }

    function enabledCount(layouts, filter) {
        let count = 0;
        if (!layouts) {
            return count;
        }

        for (let i = 0; i < layouts.count; ++i) {
            const layout = layouts.get(i);
            if (layoutMatches(layout, filter) && kcm.enabledKeyboardLayoutIds.includes(layout.layoutId)) {
                ++count;
            }
        }
        return count;
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
                placeholderText: i18n("Filter languages…")
                Accessible.name: i18n("Filter languages")

                Layout.fillWidth: true

                onTextChanged: {
                    root.searchText = text;
                    searchField.forceActiveFocus();
                }
            }

            QQC2.CheckBox {
                Layout.topMargin: Kirigami.Units.smallSpacing

                text: i18nc("@option:check", "Show all layouts")
                checked: !kcm.keyboardLayoutFormFactorFilterEnabled

                Layout.fillWidth: true

                onCheckedChanged: {
                    kcm.keyboardLayoutFormFactorFilterEnabled = !checked;
                    checked = Qt.binding(() => !kcm.keyboardLayoutFormFactorFilterEnabled);
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

    delegate: ColumnLayout {
        id: groupDelegate

        required property string groupId
        required property string name
        required property string description
        required property var layouts

        width: root.width
        height: visible ? implicitHeight : 0
        visible: root.groupVisible(groupDelegate.name, groupDelegate.description, groupDelegate.layouts)
        spacing: 0

        readonly property bool expanded: root.isExpanded(groupDelegate.groupId)
        readonly property bool displayExpanded: expanded || root.searchText.length > 0
        readonly property string layoutFilter: root.groupNameMatches(groupDelegate.name, groupDelegate.description, root.searchText.toLowerCase()) ? "" : root.searchText.toLowerCase()
        readonly property int selectedCount: root.enabledCount(layouts, layoutFilter)
        readonly property int visibleLayoutCount: root.visibleLayoutCount(layouts, layoutFilter)

        QQC2.ItemDelegate {
            Layout.fillWidth: true
            text: groupDelegate.name
            onClicked: root.toggleExpanded(groupDelegate.groupId)

            contentItem: RowLayout {
                spacing: Kirigami.Units.smallSpacing

                Kirigami.Icon {
                    source: displayExpanded ? "go-down-symbolic" : "go-next-symbolic"
                    implicitWidth: Kirigami.Units.iconSizes.small
                    implicitHeight: implicitWidth
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: Kirigami.Units.smallSpacing

                    QQC2.Label {
                        Layout.fillWidth: true
                        text: groupDelegate.name
                        elide: Text.ElideRight
                    }

                    QQC2.Label {
                        Layout.fillWidth: true
                        text: {
                            const selectionText = selectedCount === 0
                                ? i18np("%1 layout", "%1 layouts", visibleLayoutCount)
                                : i18n("%1 of %2 selected", selectedCount, visibleLayoutCount);
                            return selectionText;
                        }
                        elide: Text.ElideRight
                        opacity: 0.7
                        font: Kirigami.Theme.smallFont
                    }
                }
            }
        }

        ColumnLayout {
            visible: displayExpanded
            Layout.fillWidth: true
            spacing: 0

            Repeater {
                model: groupDelegate.layouts

                delegate: QQC2.CheckDelegate {
                    id: layoutDelegate

                    required property string layoutId
                    required property string name
                    required property string description
                    Layout.fillWidth: true
                    visible: root.layoutMatches(layoutDelegate, groupDelegate.layoutFilter)
                    height: visible ? implicitHeight : 0
                    leftPadding: Kirigami.Units.gridUnit * 2
                    text: layoutDelegate.name
                    checked: kcm.enabledKeyboardLayoutIds.includes(layoutDelegate.layoutId)

                    contentItem: ColumnLayout {
                        spacing: Kirigami.Units.smallSpacing

                        QQC2.Label {
                            Layout.fillWidth: true
                            text: layoutDelegate.name
                            elide: Text.ElideRight
                        }

                        QQC2.Label {
                            Layout.fillWidth: true
                            text: layoutDelegate.description
                            visible: text.length > 0
                            elide: Text.ElideRight
                            opacity: 0.7
                            font: Kirigami.Theme.smallFont
                        }
                    }

                    onCheckedChanged: {
                        if (checked) {
                            kcm.enableKeyboardLayout(layoutDelegate.layoutId);
                        } else {
                            kcm.disableKeyboardLayout(layoutDelegate.layoutId);
                        }
                    }
                }
            }
        }
    }
}
