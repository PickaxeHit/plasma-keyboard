/*
    SPDX-FileCopyrightText: 2024 Aleix Pol i Gonzalez <aleixpol@kde.org>
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
    SPDX-FileCopyrightText: 2025 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QQuickItem>
#include <QQuickWindow>
#include <QVirtualKeyboardInputEngine>
#include <qqmlintegration.h>

#include <xkbcommon/xkbcommon.h>

#include "inputplugin.h"

class OverlayController;

class InputListenerItem : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QVirtualKeyboardInputEngine *engine WRITE setEngine)
    Q_PROPERTY(bool keyboardNavigationActive MEMBER m_keyboardNavigationActive)

    /**
     * Controller for overlay popups (diacritics, emoji, text expansion).
     *
     * Exposed to QML for connecting overlay windows.
     */
    Q_PROPERTY(OverlayController *overlayController READ overlayController CONSTANT)

public:
    InputListenerItem();

    void setEngine(QVirtualKeyboardInputEngine *engine);

    QVariant inputMethodQuery(Qt::InputMethodQuery query) const override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void inputMethodEvent(QInputMethodEvent *event) override;

    /**
     * Get the overlay controller.
     */
    OverlayController *overlayController() const;

Q_SIGNALS:
    void keyNavigationPressed(int key);
    void keyNavigationReleased(int key);

private:
    /**
     * Checks whether a physical key event should be forwarded to the input method as a keysym.
     *
     * This is used to distinguish non-textual/control keys (which should go through the Wayland
     * keysym path) from textual keys (which should be committed as text). The `Return` key is special
     * because it often carries "\n" in @p keyEvent->text() but should still be treated as a
     * control key.
     *
     * @param keyEvent The originating physical key event.
     * @param keysym The XKB keysym derived from @p keyEvent.
     * @return True if the event should be forwarded as a keysym, false if it should be committed
     *         as text.
     */
    bool shouldSimulateKeysym(const QKeyEvent *keyEvent, xkb_keysym_t keysym) const;

    InputPlugin m_input;
    OverlayController *m_overlayController = nullptr;
    bool m_keyboardNavigationActive = false;
};
