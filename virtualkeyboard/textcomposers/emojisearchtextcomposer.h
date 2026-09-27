/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "abstracttextcomposer.h"

class EmojiController;

class EmojiSearchTextComposer : public AbstractTextComposer
{
public:
    explicit EmojiSearchTextComposer(EmojiController *controller);

    bool autoCapitalizationSupported() const override;
    bool setTextCase(TextCase textCase) override;
    bool keyEvent(Qt::Key key, const QString &text) override;
    void reset() override;

private:
    EmojiController *m_controller = nullptr;
};
