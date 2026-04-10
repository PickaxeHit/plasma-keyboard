/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "abstractinputmethod.h"

#include <qqmlintegration.h>

class DirectInputMethod : public AbstractInputMethod
{
    Q_OBJECT
    QML_NAMED_ELEMENT(DirectInputMethod)

public:
    explicit DirectInputMethod(QObject *parent = nullptr);
    bool setTextCase(TextCase textCase) override;
    bool keyEvent(Qt::Key key, const QString &text) override;

private:
    QString transformedText(const QString &text) const;

    TextCase m_textCase = TextCase::Lower;
};
