/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "abstractinputmethod.h"

#include "../inputengine.h"

AbstractInputMethod::AbstractInputMethod(QObject *parent)
    : QObject(parent)
{
}

AbstractInputMethod::~AbstractInputMethod() = default;

InputEngine *AbstractInputMethod::inputEngine() const
{
    return m_inputEngine;
}

bool AbstractInputMethod::selectCandidate(int index)
{
    Q_UNUSED(index)
    return false;
}

bool AbstractInputMethod::replaceLastInput(const QString &text)
{
    Q_UNUSED(text)
    return false;
}

bool AbstractInputMethod::showsPreeditBubble() const
{
    return false;
}

void AbstractInputMethod::reset()
{
    clearComposition();
}

void AbstractInputMethod::update()
{
}

QString AbstractInputMethod::preeditText() const
{
    return inputEngine() ? inputEngine()->preeditText() : QString();
}

QString AbstractInputMethod::preeditPrefix() const
{
    return inputEngine() ? inputEngine()->preeditPrefix() : QString();
}

QStringList AbstractInputMethod::candidates() const
{
    return inputEngine() ? inputEngine()->candidates() : QStringList();
}

void AbstractInputMethod::setPreeditText(const QString &text)
{
    if (auto *engine = inputEngine()) {
        engine->setPreeditText(text);
    }
}

void AbstractInputMethod::setPreeditPrefix(const QString &text)
{
    if (auto *engine = inputEngine()) {
        engine->setPreeditPrefix(text);
    }
}

void AbstractInputMethod::setCandidates(const QStringList &candidates)
{
    if (auto *engine = inputEngine()) {
        engine->setCandidates(candidates);
    }
}

void AbstractInputMethod::clearComposition()
{
    setPreeditPrefix(QString());
    setPreeditText(QString());
    setCandidates({});
}

void AbstractInputMethod::setInputEngine(InputEngine *inputEngine)
{
    m_inputEngine = inputEngine;
}
