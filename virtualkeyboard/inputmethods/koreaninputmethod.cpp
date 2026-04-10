/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "koreaninputmethod.h"

#include "../inputengine.h"

#include <hangul.h>

namespace
{
QString stringFromUcs(const ucschar *text)
{
    if (!text) {
        return {};
    }

    QString result;
    for (int i = 0; text[i] != 0; ++i) {
        result.append(QChar(char32_t(text[i])));
    }
    return result;
}

int asciiForHangulJamo(QChar ch)
{
    switch (ch.unicode()) {
    case 0x3142:
        return 'q';
    case 0x3143:
        return 'Q';
    case 0x3148:
        return 'w';
    case 0x3149:
        return 'W';
    case 0x3137:
        return 'e';
    case 0x3138:
        return 'E';
    case 0x3131:
        return 'r';
    case 0x3132:
        return 'R';
    case 0x3145:
        return 't';
    case 0x3146:
        return 'T';
    case 0x315B:
        return 'y';
    case 0x3155:
        return 'u';
    case 0x3151:
        return 'i';
    case 0x3150:
        return 'o';
    case 0x3152:
        return 'O';
    case 0x3154:
        return 'p';
    case 0x3156:
        return 'P';
    case 0x3141:
        return 'a';
    case 0x3134:
        return 's';
    case 0x3147:
        return 'd';
    case 0x3139:
        return 'f';
    case 0x314E:
        return 'g';
    case 0x3157:
        return 'h';
    case 0x3153:
        return 'j';
    case 0x314F:
        return 'k';
    case 0x3163:
        return 'l';
    case 0x314B:
        return 'z';
    case 0x314C:
        return 'x';
    case 0x314A:
        return 'c';
    case 0x314D:
        return 'v';
    case 0x3160:
        return 'b';
    case 0x315C:
        return 'n';
    case 0x3161:
        return 'm';
    default:
        return 0;
    }
}
}

KoreanInputMethod::KoreanInputMethod(QObject *parent)
    : AbstractInputMethod(parent)
{
}

KoreanInputMethod::~KoreanInputMethod()
{
    if (m_context) {
        hangul_ic_delete(m_context);
    }
}

bool KoreanInputMethod::setTextCase(TextCase textCase)
{
    Q_UNUSED(textCase)
    return true;
}

bool KoreanInputMethod::keyEvent(Qt::Key key, const QString &text)
{
    auto *engine = inputEngine();
    if (!engine) {
        return false;
    }

    ensureContext();
    if (!m_context) {
        return engine->handleKeyCommit(key, text);
    }

    if (key == Qt::Key_Backspace) {
        if (hangul_ic_backspace(m_context)) {
            updatePreedit();
            return true;
        }
        return engine->handleKeyCommit(key, text);
    }

    if (key == Qt::Key_Escape) {
        if (hangul_ic_is_empty(m_context)) {
            return false;
        }
        hangul_ic_reset(m_context);
        updatePreedit();
        return true;
    }

    if (!text.isEmpty() && text.size() == 1) {
        const int ascii = asciiForHangulJamo(text.at(0));
        if (ascii != 0) {
            return processHangulKey(ascii, key, text);
        }
    }

    const bool flushed = flushComposition();
    return engine->handleKeyCommit(key, text) || flushed;
}

void KoreanInputMethod::reset()
{
    if (m_context) {
        hangul_ic_reset(m_context);
    }
    clearComposition();
}

void KoreanInputMethod::update()
{
    updatePreedit();
}

void KoreanInputMethod::ensureContext()
{
    if (m_context) {
        return;
    }

    m_context = hangul_ic_new("2");
}

void KoreanInputMethod::updatePreedit()
{
    if (!m_context) {
        clearComposition();
        return;
    }

    setPreeditPrefix(QString());
    setPreeditText(stringFromUcs(hangul_ic_get_preedit_string(m_context)));
    setCandidates({});
}

bool KoreanInputMethod::processHangulKey(int ascii, Qt::Key fallbackKey, const QString &fallbackText)
{
    auto *engine = inputEngine();
    if (!engine || !m_context) {
        return false;
    }

    if (!hangul_ic_process(m_context, ascii)) {
        return engine->handleKeyCommit(fallbackKey, fallbackText);
    }

    const QString commitText = stringFromUcs(hangul_ic_get_commit_string(m_context));
    if (!commitText.isEmpty()) {
        engine->commit(commitText);
    }
    updatePreedit();
    return true;
}

bool KoreanInputMethod::flushComposition()
{
    auto *engine = inputEngine();
    if (!engine || !m_context || hangul_ic_is_empty(m_context)) {
        return false;
    }

    const QString text = stringFromUcs(hangul_ic_flush(m_context));
    if (text.isEmpty()) {
        clearComposition();
        return false;
    }

    engine->commit(text);
    updatePreedit();
    return true;
}
