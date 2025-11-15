/*
    SPDX-FileCopyrightText: 2025 Aleix Pol i Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Settings
import org.kde.kirigami as Kirigami

Window {
    id: languageDialog
    title: i18n("Available Languages:")
    color: Kirigami.Theme.backgroundColor

    signal showSettings();

    function show(localeList, currentIndex) {
        languageListModel.clear()
        for (var i in localeList) {
            languageListModel.append({localeName: localeList[i], displayName: Qt.locale(localeList[i]).nativeLanguageName})
        }
        languageListView.currentIndex = currentIndex
        languageListView.positionViewAtIndex(currentIndex, ListView.Center)
        languageDialog.visible = true
    }

    ListView {
        id: languageListView
        anchors.fill: parent
        ScrollBar.horizontal: ScrollBar {}
        ScrollBar.vertical: ScrollBar {}
        model: ListModel {
            id: languageListModel
        }
        header: RowLayout {
            width: parent.width
            Kirigami.Heading {
                Layout.fillWidth: true
                text: i18n("Languages")
            }
            Button {
                icon.name: "configure"
                onClicked: languageDialog.showSettings()
            }
        }
        delegate: ItemDelegate {
            id: languageListItem
            text: displayName
            width: parent.width
            onClicked: {
                parent.ListView.view.currentIndex = index
                VirtualKeyboardSettings.locale = languageListModel.get(index).localeName
                languageDialog.visible = false
            }
        }
    }
}
