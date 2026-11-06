/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "../abstracttextcomposer.h"
#include "libpinyinadapter.h"

#include <QStringList>
#include <qqmlintegration.h>

class PinyinTextComposer : public AbstractTextComposer
{
    Q_OBJECT
    Q_PROPERTY(QString inputMode READ inputMode WRITE setInputMode NOTIFY inputModeChanged)
    /**
     * How typed letters map to pinyin: "full" (full pinyin, the default), or a double pinyin (shuangpin) scheme:
     * "zrm" (Ziranma), "ms" (Microsoft), "ziguang", "abc" (Smart ABC), "pyjj" (Pinyin Plus Plus) or "xhe" (Xiaohe).
     * Unknown values are ignored. Changing it commits any composition in progress.
     */
    Q_PROPERTY(QString pinyinScheme READ pinyinScheme WRITE setPinyinScheme NOTIFY pinyinSchemeChanged)
    /**
     * Whether the current scheme is a double pinyin scheme (each syllable is two keys, and ' is not used).
     */
    Q_PROPERTY(bool doublePinyin READ doublePinyin NOTIFY pinyinSchemeChanged)
    /**
     * Whether the current scheme needs a ';' key (the final "ing" in the Microsoft and Ziguang schemes).
     */
    Q_PROPERTY(bool semicolonKeyRequired READ semicolonKeyRequired NOTIFY pinyinSchemeChanged)
    /**
     * When true, pinyinScheme is fixed by the layout that created this composer (for example the layout for
     * one double pinyin scheme) and the keyboard scheme setting is not applied to it.
     */
    Q_PROPERTY(bool pinyinSchemeLocked READ pinyinSchemeLocked WRITE setPinyinSchemeLocked)
    QML_NAMED_ELEMENT(PinyinTextComposer)

public:
    explicit PinyinTextComposer(QObject *parent = nullptr);
    ~PinyinTextComposer() override;
    QString inputMode() const;
    void setInputMode(const QString &inputMode);
    QString pinyinScheme() const;
    void setPinyinScheme(const QString &scheme);
    bool doublePinyin() const;
    bool semicolonKeyRequired() const;
    bool pinyinSchemeLocked() const;
    void setPinyinSchemeLocked(bool locked);
    bool autoCapitalizationSupported() const override;
    bool setTextCase(TextCase textCase) override;
    bool keyEvent(Qt::Key key, const QString &text) override;
    bool selectCandidate(int index) override;
    bool showsPreeditBubble() const override;
    void reset() override;

private:
    bool commitCurrentSelection(int index = 0);
    bool commitSurface();
    void refreshState();
    QString transformedText(const QString &text) const;

    LibPinyinAdapter m_pinyin;
    bool m_pinyinSchemeLocked = false;
    TextCase m_textCase = TextCase::Lower;
    QString m_inputMode = QStringLiteral("pinyin");

Q_SIGNALS:
    void inputModeChanged();
    void pinyinSchemeChanged();
};
