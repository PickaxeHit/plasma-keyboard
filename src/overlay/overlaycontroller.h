/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "candidatemodel.h"
#include "overlaytrigger.h"

#include <QKeyEvent>
#include <QObject>
#include <QTimer>
#include <qqmlintegration.h>

class InputPlugin;

/**
 * Central controller for overlay popups (diacritics, emoji, text expansion).
 *
 * Manages trigger registration, event dispatch, overlay lifecycle, and candidate population.
 * This class bridges C++ input handling with QML overlay views.
 *
 * Usage:
 * 1. Register triggers with registerTrigger()
 * 2. Feed input events via processKeyPress/Release/PreeditChanged/TextCommitted
 * 3. Connect to overlayRequested/overlayClosed signals in QML
 * 4. Call commitCandidate() when user selects an option
 */
class OverlayController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("OverlayController is created in C++ and passed to QML.")

    /**
     * Whether an overlay popup is currently shown.
     */
    Q_PROPERTY(bool overlayVisible READ overlayVisible NOTIFY overlayVisibleChanged)

    /**
     * The trigger ID of the currently active overlay.
     */
    Q_PROPERTY(QString activeTriggerId READ activeTriggerId NOTIFY activeTriggerIdChanged)

    /**
     * The base text that triggered the overlay (e.g., "a" for diacritics).
     */
    Q_PROPERTY(QString pendingText READ pendingText NOTIFY pendingTextChanged)

    /**
     * Model of candidates for the current overlay.
     */
    Q_PROPERTY(CandidateModel *candidateModel READ candidateModel CONSTANT)

public:
    explicit OverlayController(InputPlugin *inputPlugin, QObject *parent = nullptr);
    ~OverlayController() override;

    /**
     * Register a trigger strategy.
     *
     * Takes ownership of the trigger. Multiple triggers can be active simultaneously;
     * they are evaluated in registration order.
     *
     * @param trigger The trigger to register.
     */
    void registerTrigger(OverlayTrigger *trigger);

    /**
     * Process a key press event.
     *
     * @param event The key event.
     * @return True if the event was consumed.
     */
    bool processKeyPress(QKeyEvent *event);

    /**
     * Process a key release event.
     *
     * @param event The key event.
     * @return True if the event was consumed.
     */
    bool processKeyRelease(QKeyEvent *event);

    /**
     * Process a preedit text change.
     *
     * @param preedit The new preedit text.
     * @return True if an overlay action was triggered.
     */
    bool processPreeditChanged(const QString &preedit);

    /**
     * Process committed text.
     *
     * @param text The committed text.
     * @return True if an overlay action was triggered.
     */
    bool processTextCommitted(const QString &text);

    bool overlayVisible() const;
    QString activeTriggerId() const;
    QString pendingText() const;
    CandidateModel *candidateModel() const;

    /**
     * Pending key code for release swallowing.
     */
    int pendingKey() const;

    /**
     * Get the associated InputPlugin for commits.
     */
    InputPlugin *inputPlugin() const;

public Q_SLOTS:
    /**
     * Commit the candidate at the given index.
     *
     * @param index Row index in the candidate model.
     */
    void commitCandidate(int index);

    /**
     * Commit arbitrary text.
     *
     * @param text The text to commit.
     */
    void commitText(const QString &text);

    /**
     * Cancel the current overlay without committing.
     */
    void cancelOverlay();

    /**
     * Open the overlay with the given trigger and candidates.
     *
     * @param triggerId The trigger that activated.
     * @param baseText The base/pending text.
     * @param candidates The candidate options.
     */
    void openOverlay(const QString &triggerId, const QString &baseText, const QStringList &candidates);

Q_SIGNALS:
    /**
     * Emitted when an overlay should be shown.
     *
     * @param triggerId Which trigger type is active.
     * @param baseText The text that triggered the overlay.
     */
    void overlayRequested(const QString &triggerId, const QString &baseText);

    /**
     * Emitted when the overlay should be hidden.
     */
    void overlayClosed();

    void overlayVisibleChanged();
    void activeTriggerIdChanged();
    void pendingTextChanged();

private Q_SLOTS:
    void handleTimerExpired();

private:
    void executeAction(const OverlayTriggerResult &result, OverlayTrigger *trigger);
    void resetState();

    InputPlugin *m_inputPlugin = nullptr;
    QList<OverlayTrigger *> m_triggers;
    CandidateModel *m_candidateModel = nullptr;

    QTimer m_holdTimer;
    bool m_overlayVisible = false;
    QString m_activeTriggerId;
    QString m_pendingText;
    int m_pendingKey = 0;
    bool m_swallowNextRelease = false;
    int m_ignoreReleaseKey = 0;

    /**
     * Tracks whether the pending key was released while the overlay was still open.
     *
     * This scenario is likely when the user long-presses a key to open the overlay, then
     * releases the key after the overlay is shown but before selecting a candidate.
     */
    bool m_pendingKeyReleased = false;

    /** The trigger that is currently timing (for long-press). */
    OverlayTrigger *m_pendingTrigger = nullptr;
};
