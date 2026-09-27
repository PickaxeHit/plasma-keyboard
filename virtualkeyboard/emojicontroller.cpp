/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "emojicontroller.h"

#include "inputengine.h"
#include "textcomposers/emojisearchtextcomposer.h"

#include <QTextBoundaryFinder>
#include <QTimer>

EmojiController::EmojiController(InputEngine *inputEngine, QObject *parent)
    : QObject(parent)
    , m_inputEngine(inputEngine)
    , m_searchComposer(new EmojiSearchTextComposer(this))
{
    Q_ASSERT(m_inputEngine);

    connect(m_inputEngine, &InputEngine::inputMethodHintsChanged, this, [this] {
        Q_EMIT availableChanged();
        if (!available()) {
            close();
        }
    });
    connect(m_inputEngine, &InputEngine::textComposerChanged, this, [this] {
        if (!m_searchActive || m_inputEngine->textComposer() == m_searchComposer) {
            return;
        }

        m_previousComposer = m_inputEngine->textComposer();
        QTimer::singleShot(0, this, &EmojiController::ensureSearchComposer);
    });
}

EmojiController::~EmojiController()
{
    stopSearch();
}

bool EmojiController::active() const
{
    return m_active;
}

bool EmojiController::searchActive() const
{
    return m_searchActive;
}

QString EmojiController::query() const
{
    return m_query;
}

bool EmojiController::available() const
{
    const auto unavailableHints = Qt::ImhHiddenText | Qt::ImhSensitiveData | Qt::ImhDate | Qt::ImhTime | Qt::ImhDigitsOnly | Qt::ImhFormattedNumbersOnly
        | Qt::ImhUppercaseOnly | Qt::ImhLowercaseOnly | Qt::ImhDialableCharactersOnly | Qt::ImhEmailCharactersOnly | Qt::ImhUrlCharactersOnly
        | Qt::ImhLatinOnly;
    return !m_inputEngine->inputMethodHints().testAnyFlags(unavailableHints);
}

void EmojiController::open()
{
    if (m_active || !available()) {
        return;
    }

    m_inputEngine->commit();
    m_active = true;
    Q_EMIT activeChanged();
}

void EmojiController::close()
{
    if (!m_active) {
        return;
    }

    stopSearch();
    m_active = false;
    Q_EMIT activeChanged();
}

void EmojiController::startSearch()
{
    if (!m_active || m_searchActive) {
        return;
    }

    m_previousComposer = m_inputEngine->textComposer();
    m_searchActive = true;
    Q_EMIT searchActiveChanged();
    ensureSearchComposer();
}

void EmojiController::stopSearch()
{
    if (!m_searchActive) {
        clearQuery();
        return;
    }

    m_searchActive = false;
    Q_EMIT searchActiveChanged();
    restorePreviousComposer();
    clearQuery();
}

void EmojiController::ensureSearchComposer()
{
    if (m_searchActive && m_inputEngine->textComposer() != m_searchComposer) {
        m_inputEngine->setTextComposer(m_searchComposer);
    }
}

void EmojiController::clearQuery()
{
    setQuery(QString());
}

void EmojiController::commitEmoji(const QString &emoji)
{
    if (!m_active || emoji.isEmpty()) {
        return;
    }

    m_inputEngine->commit(emoji);
}

void EmojiController::appendQueryText(const QString &text)
{
    if (!m_searchActive || text.isEmpty()) {
        return;
    }

    setQuery(m_query + text);
}

void EmojiController::removeLastQueryCharacter()
{
    if (!m_searchActive || m_query.isEmpty()) {
        return;
    }

    QTextBoundaryFinder finder(QTextBoundaryFinder::Grapheme, m_query);
    finder.toEnd();
    const qsizetype previousBoundary = finder.toPreviousBoundary();
    setQuery(previousBoundary < 0 ? QString() : m_query.first(previousBoundary));
}

void EmojiController::setQuery(const QString &query)
{
    if (m_query == query) {
        return;
    }

    m_query = query;
    Q_EMIT queryChanged();
}

void EmojiController::restorePreviousComposer()
{
    if (m_inputEngine->textComposer() != m_searchComposer) {
        return;
    }

    m_inputEngine->setTextComposer(m_previousComposer);
    m_previousComposer.clear();
}
