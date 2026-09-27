/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "emojisearchtextcomposer.h"

#include "../emojicontroller.h"

EmojiSearchTextComposer::EmojiSearchTextComposer(EmojiController *controller)
    : AbstractTextComposer(controller)
    , m_controller(controller)
{
    Q_ASSERT(m_controller);
}

bool EmojiSearchTextComposer::autoCapitalizationSupported() const
{
    return false;
}

bool EmojiSearchTextComposer::setTextCase(TextCase textCase)
{
    Q_UNUSED(textCase)
    return true;
}

bool EmojiSearchTextComposer::keyEvent(Qt::Key key, const QString &text)
{
    switch (key) {
    case Qt::Key_Backspace:
    case Qt::Key_Delete:
        m_controller->removeLastQueryCharacter();
        return true;
    case Qt::Key_Space:
        if (!m_controller->query().isEmpty() && !m_controller->query().endsWith(QLatin1Char(' '))) {
            m_controller->appendQueryText(QStringLiteral(" "));
        }
        return true;
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Tab:
        return true;
    default:
        break;
    }

    if (text.isEmpty()) {
        return false;
    }

    m_controller->appendQueryText(text);
    return true;
}

void EmojiSearchTextComposer::reset()
{
    if (m_controller->searchActive()) {
        m_controller->clearQuery();
    }
}
