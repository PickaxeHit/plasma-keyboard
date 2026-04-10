/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "inputengine.h"

#include "inputbackend.h"
#include "inputmethods/abstractinputmethod.h"

#include <QLocale>

InputEngine::InputEngine(InputBackend *backend, QObject *parent)
    : QObject(parent)
    , m_backend(backend)
    , m_locale(QLocale().name())
{
    Q_ASSERT(m_backend);
    connectBackend();
    connect(this, &InputEngine::uppercaseChanged, this, [this] {
        if (!m_inputMethod) {
            return;
        }
        m_inputMethod->setTextCase(uppercase() ? AbstractInputMethod::TextCase::Upper : AbstractInputMethod::TextCase::Lower);
    });
    connect(this, &InputEngine::inputMethodChanged, this, [this] {
        if (!m_inputMethod) {
            return;
        }
        m_inputMethod->setTextCase(uppercase() ? AbstractInputMethod::TextCase::Upper : AbstractInputMethod::TextCase::Lower);
    });
}

AbstractInputMethod *InputEngine::inputMethod() const
{
    return m_inputMethod;
}

void InputEngine::setInputMethod(AbstractInputMethod *inputMethod)
{
    if (m_inputMethod == inputMethod) {
        return;
    }

    if (m_inputMethod) {
        m_inputMethod->reset();
        m_inputMethod->setParent(this);
    }

    m_inputMethod = inputMethod;
    if (m_inputMethod) {
        m_inputMethod->setParent(this);
        m_inputMethod->setInputEngine(this);
        m_inputMethod->setTextCase(uppercase() ? AbstractInputMethod::TextCase::Upper : AbstractInputMethod::TextCase::Lower);
    }
    Q_EMIT inputMethodChanged();
}

bool InputEngine::shiftActive() const
{
    return m_shiftActive;
}

void InputEngine::setShiftActive(bool active)
{
    if (m_shiftActive == active) {
        return;
    }

    m_shiftActive = active;
    if (!m_shiftActive && m_backend->pressedKeys().contains(Qt::Key_Shift)) {
        m_backend->sendKeyPressed(Qt::Key_Shift, false);
    }
    Q_EMIT shiftActiveChanged();
    Q_EMIT uppercaseChanged();
}

bool InputEngine::capsLockActive() const
{
    return m_capsLockActive;
}

void InputEngine::setCapsLockActive(bool active)
{
    if (m_capsLockActive == active) {
        return;
    }

    m_capsLockActive = active;
    Q_EMIT capsLockActiveChanged();
    Q_EMIT uppercaseChanged();
}

bool InputEngine::uppercase() const
{
    return m_shiftActive || m_capsLockActive;
}

QString InputEngine::preeditText() const
{
    return m_preeditText;
}

void InputEngine::setPreeditText(const QString &text)
{
    if (m_preeditText == text) {
        return;
    }

    m_preeditText = text;
    m_backend->setPreeditText(text);
    Q_EMIT preeditTextChanged();
}

QString InputEngine::preeditPrefix() const
{
    return m_preeditPrefix;
}

void InputEngine::setPreeditPrefix(const QString &text)
{
    if (m_preeditPrefix == text) {
        return;
    }

    m_preeditPrefix = text;
    Q_EMIT preeditPrefixChanged();
}

QStringList InputEngine::candidates() const
{
    return m_candidates;
}

void InputEngine::setCandidates(const QStringList &candidates)
{
    if (m_candidates == candidates) {
        return;
    }

    m_candidates = candidates;
    Q_EMIT candidatesChanged();
    Q_EMIT wordCandidateListVisibleHintChanged();
}

QString InputEngine::locale() const
{
    return m_locale;
}

void InputEngine::setLocale(const QString &locale)
{
    if (m_locale == locale) {
        return;
    }

    m_locale = locale;
    Q_EMIT localeChanged();
}

int InputEngine::anchorPosition() const
{
    return utf16PositionFromUtf8Offset(m_backend->anchorPositionUtf8());
}

int InputEngine::cursorPosition() const
{
    return utf16PositionFromUtf8Offset(m_backend->cursorPositionUtf8());
}

Qt::InputMethodHints InputEngine::inputMethodHints() const
{
    return m_backend->inputMethodHints();
}

QString InputEngine::surroundingText() const
{
    return m_backend->surroundingText();
}

QString InputEngine::selectedText() const
{
    const QByteArray utf8 = surroundingText().toUtf8();
    const int start = qMin(m_backend->cursorPositionUtf8(), m_backend->anchorPositionUtf8());
    const int end = qMax(m_backend->cursorPositionUtf8(), m_backend->anchorPositionUtf8());
    return QString::fromUtf8(utf8.mid(start, end - start));
}

QList<int> InputEngine::pressedKeys() const
{
    return m_backend->pressedKeys();
}

bool InputEngine::preeditBubbleVisibleHint() const
{
    return m_inputMethod && m_inputMethod->showsPreeditBubble();
}

WordCandidateListModel *InputEngine::wordCandidateListModel() const
{
    if (!m_wordCandidateListModel) {
        m_wordCandidateListModel = new WordCandidateListModel(const_cast<InputEngine *>(this), const_cast<InputEngine *>(this));
    }
    return m_wordCandidateListModel;
}

bool InputEngine::wordCandidateListVisibleHint() const
{
    return !m_candidates.isEmpty();
}

bool InputEngine::sendInputMethodKey(int key, const QString &text)
{
    if (!m_inputMethod) {
        return false;
    }

    if (!m_backend->pressedKeys().isEmpty()) {
        return sendDirectKeyClick(key);
    }

    return m_inputMethod->keyEvent(Qt::Key(key), text);
}

bool InputEngine::sendDirectKey(int key, bool pressed)
{
    if (!m_backend->sendKeyPressed(key, pressed)) {
        return false;
    }

    return true;
}

bool InputEngine::isKeyPressed(int key) const
{
    return m_backend->pressedKeys().contains(key);
}

bool InputEngine::selectCandidate(int index)
{
    if (!m_inputMethod) {
        return false;
    }

    return m_inputMethod->selectCandidate(index);
}

void InputEngine::commit()
{
    if (!m_preeditText.isEmpty()) {
        m_backend->commitText(m_preeditText);
        clearCompositionState(true);
    }
}

void InputEngine::commit(const QString &text, int replaceFrom, int replaceLength)
{
    if (replaceLength != 0) {
        m_backend->deleteSurroundingText(replaceFrom, replaceLength);
    }

    if (!text.isEmpty()) {
        m_backend->commitText(text);
    }

    clearCompositionState(true);
}

void InputEngine::clear()
{
    clearCompositionState(true);
}

bool InputEngine::handleKeyCommit(int key, const QString &text)
{
    switch (key) {
    case Qt::Key_Backspace:
        if (m_backend->sendKeyClick(key)) {
            return true;
        }
        m_backend->deleteSurroundingText(-1, 1);
        m_backend->commitText(QString());
        return true;
    case Qt::Key_Delete:
        if (m_backend->sendKeyClick(key)) {
            return true;
        }
        m_backend->deleteSurroundingText(0, 1);
        m_backend->commitText(QString());
        return true;
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_Home:
    case Qt::Key_End:
        return m_backend->sendKeyClick(key);
    case Qt::Key_Return:
    case Qt::Key_Enter:
        if (m_backend->sendKeyClick(key)) {
            return true;
        }
        commit(QStringLiteral("\n"));
        return true;
    case Qt::Key_Tab:
        if (m_backend->sendKeyClick(key)) {
            return true;
        }
        commit(QStringLiteral("\t"));
        return true;
    case Qt::Key_Space:
        if (m_backend->sendKeyClick(key)) {
            return true;
        }
        commit(QStringLiteral(" "));
        return true;
    default:
        break;
    }

    if (text.isEmpty()) {
        return false;
    }

    commit(text);
    if (m_shiftActive && !m_capsLockActive) {
        setShiftActive(false);
    }
    return true;
}

void InputEngine::connectBackend()
{
    if (!m_backend->parent()) {
        m_backend->setParent(this);
    }

    connect(m_backend, &InputBackend::surroundingTextChanged, this, [this] {
        Q_EMIT surroundingTextChanged();
        Q_EMIT selectedTextChanged();
        Q_EMIT anchorPositionChanged();
        Q_EMIT cursorPositionChanged();
    });
    connect(m_backend, &InputBackend::inputMethodHintsChanged, this, [this] {
        Q_EMIT inputMethodHintsChanged();
    });
    connect(m_backend, &InputBackend::pressedKeysChanged, this, [this] {
        Q_EMIT pressedKeysChanged();
    });
    connect(m_backend, &InputBackend::activeChanged, this, [this] {
        Q_EMIT surroundingTextChanged();
        Q_EMIT selectedTextChanged();
        Q_EMIT anchorPositionChanged();
        Q_EMIT cursorPositionChanged();
        Q_EMIT inputMethodHintsChanged();
    });
    connect(m_backend, &InputBackend::resetRequested, this, [this] {
        if (m_inputMethod) {
            m_inputMethod->reset();
        } else {
            clear();
        }
    });
}

void InputEngine::clearCompositionState(bool updateBackend)
{
    const bool hadPreedit = !m_preeditText.isEmpty();
    const bool hadCandidates = !m_candidates.isEmpty();

    if (updateBackend && hadPreedit) {
        m_backend->setPreeditText(QString());
    }

    if (hadPreedit) {
        m_preeditText.clear();
        Q_EMIT preeditTextChanged();
    }
    if (!m_preeditPrefix.isEmpty()) {
        m_preeditPrefix.clear();
        Q_EMIT preeditPrefixChanged();
    }
    if (hadCandidates) {
        m_candidates.clear();
        Q_EMIT candidatesChanged();
        Q_EMIT wordCandidateListVisibleHintChanged();
    }
}

bool InputEngine::sendDirectKeyClick(int key)
{
    return m_backend->sendKeyClick(key);
}

int InputEngine::utf16PositionFromUtf8Offset(uint32_t offset) const
{
    return QString::fromUtf8(surroundingText().toUtf8().first(offset)).size();
}
