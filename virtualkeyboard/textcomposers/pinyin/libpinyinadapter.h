/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QStringList>

typedef struct _pinyin_instance_t pinyin_instance_t;
typedef struct _lookup_candidate_t lookup_candidate_t;

/**
 * Communication and state for libpinyin support
 */
class LibPinyinAdapter
{
public:
    /**
     * How typed letters map to pinyin syllables.
     *
     * Full is regular full pinyin. The others are double pinyin (shuangpin) schemes, where each syllable is
     * typed as two keys (initial + final).
     */
    enum class Scheme {
        Full,
        Ziranma, // 自然码
        Microsoft, // 微软双拼
        Ziguang, // 紫光双拼
        SmartAbc, // 智能 ABC 双拼
        PinyinPlusPlus, // 拼音加加双拼
        Xiaohe, // 小鹤双拼
    };

    LibPinyinAdapter() = default;
    ~LibPinyinAdapter();
    LibPinyinAdapter(const LibPinyinAdapter &) = delete;
    LibPinyinAdapter &operator=(const LibPinyinAdapter &) = delete;

    /**
     * Stable identifiers used in configuration: "full", "zrm", "ms", "ziguang", "abc", "pyjj", "xhe".
     */
    static QString schemeId(Scheme scheme);
    static bool schemeFromId(const QString &id, Scheme &scheme);

    Scheme scheme() const;
    /**
     * Changes the scheme. Any composition in progress is discarded, since the typed keys would mean something else.
     */
    void setScheme(Scheme scheme);
    bool isDoublePinyin() const;
    /**
     * Whether the ';' key is part of the scheme (it is the final "ing" in the Microsoft and Ziguang schemes).
     */
    bool usesSemicolon() const;

    bool appendText(const QString &text);
    bool removeLastInput();
    QString takeCommitText(int index = 0);
    QString takeSurface();
    void reset();

    bool hasComposition() const;
    QString preeditText() const;
    QStringList candidates() const;

private:
    bool acceptsText(const QString &text) const;
    bool rebuildLookup();
    void refreshLookup();

    Scheme m_scheme = Scheme::Full;
    pinyin_instance_t *m_instance = nullptr;

    // Null entries represent candidates that commit the entire surface
    QList<lookup_candidate_t *> m_lookupCandidates;
    QString m_surface; // the raw text being composed (e.g. "nihao")
    QStringList m_candidates;
};
