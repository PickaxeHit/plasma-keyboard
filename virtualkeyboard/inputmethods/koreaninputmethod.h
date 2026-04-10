/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "abstractinputmethod.h"

#include <QStringList>
#include <qqmlintegration.h>

typedef struct _HangulInputContext HangulInputContext;

class KoreanInputMethod : public AbstractInputMethod
{
    Q_OBJECT
    QML_NAMED_ELEMENT(KoreanInputMethod)

public:
    explicit KoreanInputMethod(QObject *parent = nullptr);
    ~KoreanInputMethod() override;

    bool setTextCase(TextCase textCase) override;
    bool keyEvent(Qt::Key key, const QString &text) override;
    void reset() override;
    void update() override;

private:
    void ensureContext();
    void updatePreedit();
    bool processHangulKey(int ascii, Qt::Key fallbackKey, const QString &fallbackText);
    bool flushComposition();

    HangulInputContext *m_context = nullptr;
};
