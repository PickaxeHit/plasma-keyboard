/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "zhuyininputmethod.h"

#include "../inputengine.h"
#include "config-plasma-keyboard.h"

#include <QDir>
#include <QStandardPaths>

#include <chewing/chewing.h>

namespace
{
QString chewingString(const char *text)
{
    return text ? QString::fromUtf8(text) : QString();
}

QString chewingUserDir()
{
    const QString userDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QStringLiteral("/libchewing");
    QDir().mkpath(userDir);
    return userDir;
}

ChewingContext *createChewingContext()
{
    const QString systemDir = QString::fromUtf8(PLASMA_KEYBOARD_LIBCHEWING_DATADIR);
    const QString userDir = chewingUserDir();
    return chewing_new2(systemDir.toUtf8().constData(), userDir.toUtf8().constData(), nullptr, nullptr);
}

int defaultStrokeForChar(QChar ch)
{
    switch (ch.unicode()) {
    case 0x3105:
        return '1';
    case 0x3109:
        return '2';
    case 0x02c7:
        return '3';
    case 0x02cb:
        return '4';
    case 0x3113:
        return '5';
    case 0x02ca:
        return '6';
    case 0x02d9:
        return '7';
    case 0x311a:
        return '8';
    case 0x311e:
        return '9';
    case 0x3122:
        return '0';
    case 0x3106:
        return 'q';
    case 0x310a:
        return 'w';
    case 0x310d:
        return 'e';
    case 0x3110:
        return 'r';
    case 0x3114:
        return 't';
    case 0x3117:
        return 'y';
    case 0x3127:
        return 'u';
    case 0x311b:
        return 'i';
    case 0x311f:
        return 'o';
    case 0x3123:
        return 'p';
    case 0x3107:
        return 'a';
    case 0x310b:
        return 's';
    case 0x310e:
        return 'd';
    case 0x3111:
        return 'f';
    case 0x3115:
        return 'g';
    case 0x3118:
        return 'h';
    case 0x3128:
        return 'j';
    case 0x311c:
        return 'k';
    case 0x3120:
        return 'l';
    case 0x3108:
        return 'z';
    case 0x310c:
        return 'x';
    case 0x310f:
        return 'c';
    case 0x3112:
        return 'v';
    case 0x3116:
        return 'b';
    case 0x3119:
        return 'n';
    case 0x3129:
        return 'm';
    case 0x311d:
        return ',';
    case 0x3121:
        return '.';
    case 0x3125:
        return '/';
    case 0x3124:
        return ';';
    case 0x3126:
        return '-';
    default:
        return 0;
    }
}
}

ZhuyinInputMethod::ZhuyinInputMethod(QObject *parent)
    : AbstractInputMethod(parent)
{
}

ZhuyinInputMethod::~ZhuyinInputMethod()
{
    if (m_context) {
        chewing_delete(m_context);
    }
}

bool ZhuyinInputMethod::setTextCase(TextCase textCase)
{
    Q_UNUSED(textCase)
    return true;
}

bool ZhuyinInputMethod::keyEvent(Qt::Key key, const QString &text)
{
    auto *engine = inputEngine();
    if (!engine) {
        return false;
    }

    ensureContext();
    if (!m_context) {
        return engine->handleKeyCommit(key, text);
    }

    switch (key) {
    case Qt::Key_Backspace:
        if (hasComposition()) {
            if (!m_strokeBuffer.isEmpty()) {
                m_strokeBuffer.chop(1);
            }
            chewing_handle_Backspace(m_context);
            commitPendingText();
            refreshState();
            return true;
        }
        return engine->handleKeyCommit(key, text);
    case Qt::Key_Escape:
        if (!hasComposition()) {
            return false;
        }
        chewing_Reset(m_context);
        refreshState();
        return true;
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Space:
        if (hasComposition()) {
            return commitPreeditBuffer();
        }
        return engine->handleKeyCommit(key, text);
    default:
        break;
    }

    const int stroke = strokeForText(text);
    if (stroke != 0) {
        m_strokeBuffer.append(char(stroke));
        chewing_handle_Default(m_context, stroke);
        commitPendingText();
        refreshState();
        return true;
    }

    if (hasComposition()) {
        const bool committed = commitPreeditBuffer();
        return engine->handleKeyCommit(key, text) || committed;
    }

    return engine->handleKeyCommit(key, text);
}

bool ZhuyinInputMethod::selectCandidate(int index)
{
    ensureContext();
    if (!m_context) {
        return false;
    }

    if (m_candidatesUsePreviewContext) {
        chewing_handle_Space(m_context);
        chewing_handle_Down(m_context);
    } else if (chewing_handle_Down(m_context) != 0) {
        return false;
    }
    if (chewing_cand_choose_by_index(m_context, index) != 0) {
        refreshState();
        return false;
    }
    return commitPreeditBuffer();
}

bool ZhuyinInputMethod::showsPreeditBubble() const
{
    return true;
}

void ZhuyinInputMethod::reset()
{
    if (m_context) {
        chewing_Reset(m_context);
    }
    m_strokeBuffer.clear();
    m_candidatesUsePreviewContext = false;
    clearComposition();
}

void ZhuyinInputMethod::update()
{
    refreshState();
}

void ZhuyinInputMethod::ensureContext()
{
    if (m_context) {
        return;
    }

    m_context = createChewingContext();
    if (!m_context) {
        return;
    }

    chewing_set_KBType(m_context, chewingLayoutId());
    chewing_set_ChiEngMode(m_context, CHINESE_MODE);
    chewing_set_ShapeMode(m_context, HALFSHAPE_MODE);
    chewing_set_easySymbolInput(m_context, 0);
    chewing_set_spaceAsSelection(m_context, 0);
    chewing_set_candPerPage(m_context, 10);
}

void ZhuyinInputMethod::refreshState()
{
    if (!m_context) {
        clearComposition();
        return;
    }

    const QString buffer = chewingString(chewing_buffer_String_static(m_context));
    const QString bopomofo = chewingString(chewing_bopomofo_String_static(m_context));

    if (buffer.isEmpty() && bopomofo.isEmpty()) {
        clearComposition();
        return;
    }

    const QStringList candidateList = currentCandidates();
    setCandidates(candidateList);

    if (!buffer.isEmpty() && !bopomofo.isEmpty()) {
        setPreeditPrefix(buffer);
        setPreeditText(buffer + bopomofo);
        return;
    }

    setPreeditPrefix(QString());
    setPreeditText(!bopomofo.isEmpty() ? bopomofo : buffer);
}

bool ZhuyinInputMethod::hasComposition() const
{
    return m_context && (chewing_buffer_Check(m_context) || chewing_bopomofo_Check(m_context));
}

bool ZhuyinInputMethod::commitPendingText()
{
    auto *engine = inputEngine();
    if (!engine || !m_context || !chewing_commit_Check(m_context)) {
        return false;
    }

    const QString text = chewingString(chewing_commit_String_static(m_context));
    chewing_ack(m_context);
    if (text.isEmpty()) {
        return false;
    }

    engine->commit(text);
    m_strokeBuffer.clear();
    return true;
}

bool ZhuyinInputMethod::commitPreeditBuffer()
{
    if (!m_context) {
        return false;
    }

    chewing_commit_preedit_buf(m_context);
    const bool committed = commitPendingText();
    refreshState();
    return committed;
}

QStringList ZhuyinInputMethod::currentCandidates()
{
    if (!m_context) {
        return {};
    }

    m_candidatesUsePreviewContext = false;
    if (chewing_handle_Down(m_context) != 0) {
        return previewCandidatesForCurrentStrokeBuffer();
    }

    QStringList candidateList;
    const int totalChoices = chewing_cand_TotalChoice(m_context);
    candidateList.reserve(totalChoices);
    for (int i = 0; i < totalChoices; ++i) {
        const QString candidate = chewingString(chewing_cand_string_by_index_static(m_context, i));
        if (!candidate.isEmpty()) {
            candidateList.append(candidate);
        }
    }

    chewing_cand_close(m_context);
    candidateList.removeDuplicates();
    if (candidateList.isEmpty()) {
        return previewCandidatesForCurrentStrokeBuffer();
    }
    return candidateList;
}

QStringList ZhuyinInputMethod::previewCandidatesForCurrentStrokeBuffer() const
{
    if (m_strokeBuffer.isEmpty()) {
        return {};
    }

    std::unique_ptr<ChewingContext, decltype(&chewing_delete)> previewContext(createPreviewContext(), &chewing_delete);
    if (!previewContext) {
        return {};
    }

    chewing_handle_Space(previewContext.get());
    if (chewing_handle_Down(previewContext.get()) != 0) {
        return {};
    }

    QStringList candidateList;
    const int totalChoices = chewing_cand_TotalChoice(previewContext.get());
    candidateList.reserve(totalChoices);
    for (int i = 0; i < totalChoices; ++i) {
        const QString candidate = chewingString(chewing_cand_string_by_index_static(previewContext.get(), i));
        if (!candidate.isEmpty()) {
            candidateList.append(candidate);
        }
    }
    candidateList.removeDuplicates();
    if (!candidateList.isEmpty()) {
        const_cast<ZhuyinInputMethod *>(this)->m_candidatesUsePreviewContext = true;
    }
    return candidateList;
}

ChewingContext *ZhuyinInputMethod::createPreviewContext() const
{
    ChewingContext *context = createChewingContext();
    if (!context) {
        return nullptr;
    }

    chewing_set_KBType(context, chewingLayoutId());
    chewing_set_ChiEngMode(context, CHINESE_MODE);
    chewing_set_ShapeMode(context, HALFSHAPE_MODE);
    chewing_set_easySymbolInput(context, 0);
    chewing_set_spaceAsSelection(context, 0);
    chewing_set_candPerPage(context, 10);

    for (const char stroke : m_strokeBuffer) {
        chewing_handle_Default(context, stroke);
    }

    return context;
}

int ZhuyinInputMethod::chewingLayoutId() const
{
    return KB_DEFAULT;
}

int ZhuyinInputMethod::strokeForText(const QString &text) const
{
    if (text.size() != 1) {
        return 0;
    }

    return defaultStrokeForChar(text.at(0));
}
