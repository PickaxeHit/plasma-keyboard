/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "abstractinputmethod.h"

#include <QStringList>
#include <qqmlintegration.h>

typedef struct ChewingContext ChewingContext;

class ZhuyinInputMethod : public AbstractInputMethod
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ZhuyinInputMethod)

public:
    explicit ZhuyinInputMethod(QObject *parent = nullptr);
    ~ZhuyinInputMethod() override;

    bool setTextCase(TextCase textCase) override;
    bool keyEvent(Qt::Key key, const QString &text) override;
    bool selectCandidate(int index) override;
    bool showsPreeditBubble() const override;
    void reset() override;
    void update() override;

private:
    void ensureContext();
    void refreshState();
    bool hasComposition() const;
    bool commitPendingText();
    bool commitPreeditBuffer();
    QStringList currentCandidates();
    QStringList previewCandidatesForCurrentStrokeBuffer() const;
    ChewingContext *createPreviewContext() const;
    int chewingLayoutId() const;
    int strokeForText(const QString &text) const;

    QByteArray m_strokeBuffer;
    bool m_candidatesUsePreviewContext = false;
    ChewingContext *m_context = nullptr;
};
