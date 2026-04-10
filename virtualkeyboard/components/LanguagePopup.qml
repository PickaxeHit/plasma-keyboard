// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.plasma.keyboard.virtualkeyboard

QQC2.Popup {
    id: root

    property Item keyboardPanel
    property var keyboardController: VirtualKeyboard.keyboardController
    readonly property bool popupVisible: visible
    readonly property int itemCount: languageListView.count
    property int currentIndex: languageListView.currentIndex
    readonly property string currentLayoutId: currentIndex >= 0
        && keyboardController
        && currentIndex < keyboardController.activeLayoutIds.length
        ? keyboardController.activeLayoutIds[currentIndex]
        : ""

    signal showSettings()

    parent: keyboardPanel
    width: languageListView.rowWidth + languageListView.margins * 2
    height: Math.min(languageListView.contentHeight + languageListView.spacing + settingsButton.implicitHeight + settingsButton.Layout.topMargin,
                     languageListView.rowHeight * 6) + languageListView.margins * 2
    modal: true
    dim: false
    padding: 0
    closePolicy: QQC2.Popup.CloseOnEscape | QQC2.Popup.CloseOnPressOutsideParent

    background: PopupBackground {
        theme: BreezeConstants
    }

    TextMetrics {
        id: languageNameTextMetrics
        font.family: BreezeConstants.fontFamily
        font.weight: Font.Light
        font.pixelSize: Math.round(44 * BreezeConstants.scaleHint)
        text: "X"
    }

    function showForItem(parentItem) {
        if (!keyboardPanel || !parentItem) {
            return;
        }

        if (!keyboardController) {
            return;
        }

        currentIndex = keyboardController.currentLayoutIndex;
        languageListView.positionViewAtIndex(currentIndex, ListView.Center);

        const point = parentItem.mapToItem(keyboardPanel, 0, 0);
        const maxX = Math.max(0, keyboardPanel.width - width);
        const desiredX = point.x + (parentItem.width - width) / 2;

        x = Math.max(0, Math.min(maxX, desiredX));
        y = Math.max(0, point.y - height);
        open();
    }

    function moveSelection(delta) {
        if (itemCount <= 0) {
            return false;
        }

        const nextIndex = Math.max(0, Math.min(itemCount - 1, currentIndex + delta));
        if (nextIndex === currentIndex) {
            return false;
        }

        currentIndex = nextIndex;
        languageListView.positionViewAtIndex(currentIndex, ListView.Contain);
        return true;
    }

    function activateCurrent() {
        if (!keyboardController || currentIndex < 0 || currentIndex >= keyboardController.activeLayoutIds.length) {
            return false;
        }

        keyboardController.setCurrentLayout(keyboardController.activeLayoutIds[currentIndex]);
        close();
        return true;
    }

    contentItem: Item {
        clip: true

        ColumnLayout {
            anchors.fill: parent
            anchors.leftMargin: languageListView.margins
            anchors.rightMargin: languageListView.margins
            anchors.bottomMargin: languageListView.margins
            spacing: 0

            ListView {
                id: languageListView

                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                topMargin: margins
                bottomMargin: spacing
                spacing: 0

                readonly property real margins: 20 * BreezeConstants.scaleHint
                readonly property real rowPadding: languageNameTextMetrics.height / 3
                readonly property real rowHeight: languageNameTextMetrics.height + rowPadding * 2
                readonly property real rowWidth: languageNameTextMetrics.width * 17

                QQC2.ScrollBar.vertical: QQC2.ScrollBar {}

                model: root.keyboardController ? root.keyboardController.activeLayoutIds : []

                delegate: QQC2.ItemDelegate {
                    required property string modelData
                    property bool navigationActive: root.currentLayoutId === modelData

                    width: languageListView.rowWidth
                    height: languageListView.rowHeight
                    highlighted: root.keyboardController && (root.keyboardController.layoutId === modelData || navigationActive)
                    leftPadding: languageListView.rowPadding
                    rightPadding: languageListView.rowPadding
                    topPadding: languageListView.rowPadding
                    bottomPadding: languageListView.rowPadding

                    background: Rectangle {
                        color: {
                            if (parent.down) {
                                return BreezeConstants.popupHighlightBorderColor
                            }
                            if (parent.highlighted || (parent.hovered && !Kirigami.Settings.tabletMode)) {
                                return BreezeConstants.popupHighlightColor
                            }
                            return "transparent"
                        }
                        radius: BreezeConstants.buttonRadius
                        border.width: 1
                        border.color: {
                            if (parent.down || parent.highlighted || (parent.hovered && !Kirigami.Settings.tabletMode)) {
                                return BreezeConstants.popupHighlightBorderColor
                            }
                            return "transparent"
                        }
                    }

                    contentItem: RowLayout {
                        spacing: Math.round(12 * BreezeConstants.scaleHint)

                        QQC2.Label {
                            Layout.fillWidth: true
                            text: root.keyboardController ? root.keyboardController.layoutDisplayName(modelData) : ""
                            color: BreezeConstants.popupTextColor
                            elide: Text.ElideRight
                            font.family: BreezeConstants.fontFamily
                            font.weight: Font.Light
                            font.pixelSize: Math.round(44 * BreezeConstants.scaleHint)
                        }
                    }

                    onClicked: {
                        if (!root.keyboardController) {
                            return;
                        }

                        root.currentIndex = root.keyboardController.activeLayoutIds.indexOf(modelData);
                        root.keyboardController.setCurrentLayout(modelData);
                        root.close();
                    }
                }
            }

            Kirigami.Separator {
                Layout.fillWidth: true
            }

            QQC2.ItemDelegate {
                id: settingsButton

                Layout.topMargin: Math.round(4 * BreezeConstants.scaleHint)
                Layout.fillWidth: true
                implicitHeight: languageListView.rowHeight
                leftPadding: languageListView.rowPadding
                rightPadding: languageListView.rowPadding
                topPadding: languageListView.rowPadding
                bottomPadding: languageListView.rowPadding

                background: Rectangle {
                    color: {
                        if (settingsButton.down) {
                            return BreezeConstants.popupHighlightBorderColor
                        }
                        if (settingsButton.hovered && !Kirigami.Settings.tabletMode) {
                            return BreezeConstants.popupHighlightColor
                        }
                        return "transparent"
                    }
                    radius: BreezeConstants.buttonRadius
                    border.width: 1
                    border.color: {
                        if (settingsButton.down || (settingsButton.hovered && !Kirigami.Settings.tabletMode)) {
                            return BreezeConstants.popupHighlightBorderColor
                        }
                        return "transparent"
                    }
                }

                contentItem: RowLayout {
                    spacing: Math.round(12 * BreezeConstants.scaleHint)

                    Kirigami.Icon {
                        source: "configure"
                        implicitWidth: Math.round(44 * BreezeConstants.scaleHint)
                        implicitHeight: implicitWidth
                    }

                    QQC2.Label {
                        Layout.fillWidth: true
                        text: i18n("Settings")
                        color: BreezeConstants.popupTextColor
                        elide: Text.ElideRight
                        font.family: BreezeConstants.fontFamily
                        font.weight: Font.Light
                        font.pixelSize: Math.round(44 * BreezeConstants.scaleHint)
                    }
                }

                onClicked: {
                    root.showSettings()
                    root.close()
                }
            }
        }
    }
}
