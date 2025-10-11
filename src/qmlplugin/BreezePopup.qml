// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami

Kirigami.ShadowedRectangle {
    property BreezeConstants theme

    shadow.size: 5
    shadow.color: Qt.rgba(0, 0, 0, 0.2)

    color: theme.popupBackgroundColor
    radius: theme.popupRadius
    border {
        width: 1
        color: theme.popupBorderColor
    }
}
