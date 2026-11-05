/*
    SPDX-FileCopyrightText: 2026 Nikolay Kochulin <basiqueevangelist@yandex.ru>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QObject>

class QAction;
class InputPlugin;

/**
 * Controller for handling retyping text in different keyboard layouts.
 *
 * Currently only support manual retyping, though automatic retyping may be added if desired.
 */
class LayoutSwitch : public QObject
{
    Q_OBJECT

public:
    explicit LayoutSwitch(InputPlugin *inputPlugin, QObject *parent = nullptr);
    ~LayoutSwitch() override = default;

private Q_SLOTS:
    void manualRetype();

private:
    QAction *m_retypeAction;
    InputPlugin *m_inputPlugin;
};