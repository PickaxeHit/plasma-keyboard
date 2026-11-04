/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QObject>

class VkbdWaylandInputBackend;
class EiKeyboard;

/**
 * Sends keys unsupported by the input method protocol through KWin's EIS interface.
 *
 * Keys sent here effectively emulate the keyboard.
 */
class KWinEmulatedInput : public QObject
{
    Q_OBJECT

public:
    explicit KWinEmulatedInput(VkbdWaylandInputBackend *backend, QObject *parent = nullptr);

    bool shouldEmulateKey(int key) const;
    bool sendKeyPressed(int key, bool pressed);

private:
    bool ensureInitialized();
    bool isModifier(int key) const;
    bool supportsKey(int key) const;
    void clearPressedModifiers();
    void sendKeyboardKey(int key, bool pressed);

    EiKeyboard *m_eiKeyboard = nullptr;
    VkbdWaylandInputBackend *m_backend = nullptr;
};
