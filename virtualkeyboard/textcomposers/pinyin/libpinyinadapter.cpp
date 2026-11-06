/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "libpinyinadapter.h"

#include "config-plasma-keyboard.h"
#include "logging.h"

#include <QDir>
#include <QStandardPaths>

#include <pinyin.h>

static uint CANDIDATES_LIMIT = 64;

namespace
{
struct SchemeInfo {
    LibPinyinAdapter::Scheme scheme;
    const char *id;
};

constexpr SchemeInfo SCHEMES[] = {
    {LibPinyinAdapter::Scheme::Full, "full"},
    {LibPinyinAdapter::Scheme::Ziranma, "zrm"},
    {LibPinyinAdapter::Scheme::Microsoft, "ms"},
    {LibPinyinAdapter::Scheme::Ziguang, "ziguang"},
    {LibPinyinAdapter::Scheme::SmartAbc, "abc"},
    {LibPinyinAdapter::Scheme::PinyinPlusPlus, "pyjj"},
    {LibPinyinAdapter::Scheme::Xiaohe, "xhe"},
};

// Only meaningful for double pinyin schemes
DoublePinyinScheme libpinyinDoubleScheme(LibPinyinAdapter::Scheme scheme)
{
    switch (scheme) {
    case LibPinyinAdapter::Scheme::Ziranma:
        return DOUBLE_PINYIN_ZRM;
    case LibPinyinAdapter::Scheme::Microsoft:
        return DOUBLE_PINYIN_MS;
    case LibPinyinAdapter::Scheme::Ziguang:
        return DOUBLE_PINYIN_ZIGUANG;
    case LibPinyinAdapter::Scheme::SmartAbc:
        return DOUBLE_PINYIN_ABC;
    case LibPinyinAdapter::Scheme::PinyinPlusPlus:
        return DOUBLE_PINYIN_PYJJ;
    case LibPinyinAdapter::Scheme::Xiaohe:
        return DOUBLE_PINYIN_XHE;
    case LibPinyinAdapter::Scheme::Full:
        break;
    }
    return DOUBLE_PINYIN_DEFAULT;
}
}

static pinyin_context_t *sharedPinyinContext()
{
    static pinyin_context_t *context = []() -> pinyin_context_t * {
        const QString userDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QStringLiteral("/libpinyin");
        QDir().mkpath(userDir);

        const QString systemDir = QString::fromUtf8(PLASMA_KEYBOARD_LIBPINYIN_SYSTEM_DIR);
        pinyin_context_t *ctx = pinyin_init(systemDir.toUtf8().constData(), userDir.toUtf8().constData());
        if (!ctx) {
            qCWarning(PlasmaKeyboard) << "LibPinyinAdapter: failed to initialize libpinyin context with system directory" << systemDir;
            return nullptr;
        }

        pinyin_set_full_pinyin_scheme(ctx, FULL_PINYIN_DEFAULT);
        pinyin_set_options(ctx, USE_DIVIDED_TABLE | USE_RESPLIT_TABLE | PINYIN_INCOMPLETE | DYNAMIC_ADJUST);
        for (guint8 index = GB_DICTIONARY; index <= ADDON_DICTIONARY; ++index) {
            pinyin_load_phrase_library(ctx, index);
        }
        return ctx;
    }();

    return context;
}

LibPinyinAdapter::~LibPinyinAdapter()
{
    if (m_instance) {
        pinyin_free_instance(m_instance);
    }
}

QString LibPinyinAdapter::schemeId(Scheme scheme)
{
    for (const SchemeInfo &info : SCHEMES) {
        if (info.scheme == scheme) {
            return QString::fromLatin1(info.id);
        }
    }
    return QStringLiteral("full");
}

bool LibPinyinAdapter::schemeFromId(const QString &id, Scheme &scheme)
{
    for (const SchemeInfo &info : SCHEMES) {
        if (id == QLatin1String(info.id)) {
            scheme = info.scheme;
            return true;
        }
    }
    return false;
}

LibPinyinAdapter::Scheme LibPinyinAdapter::scheme() const
{
    return m_scheme;
}

void LibPinyinAdapter::setScheme(Scheme scheme)
{
    if (m_scheme == scheme) {
        return;
    }
    reset();
    m_scheme = scheme;
}

bool LibPinyinAdapter::isDoublePinyin() const
{
    return m_scheme != Scheme::Full;
}

bool LibPinyinAdapter::usesSemicolon() const
{
    return m_scheme == Scheme::Microsoft || m_scheme == Scheme::Ziguang;
}

bool LibPinyinAdapter::appendText(const QString &text)
{
    if (!acceptsText(text)) {
        return false;
    }
    m_surface += text;
    refreshLookup();
    return true;
}

bool LibPinyinAdapter::removeLastInput()
{
    if (!hasComposition()) {
        return false;
    }
    m_surface.chop(1);
    refreshLookup();
    return true;
}

bool LibPinyinAdapter::hasComposition() const
{
    return !m_surface.isEmpty();
}

QString LibPinyinAdapter::preeditText() const
{
    return m_surface;
}

QStringList LibPinyinAdapter::candidates() const
{
    return m_candidates;
}

QString LibPinyinAdapter::takeSurface()
{
    QString text = m_surface;
    reset();
    return text;
}

void LibPinyinAdapter::reset()
{
    m_lookupCandidates.clear();
    if (m_instance) {
        pinyin_reset(m_instance);
    }
    m_surface.clear();
    m_candidates.clear();
}

QString LibPinyinAdapter::takeCommitText(int index)
{
    QString commitText = m_candidates.value(index, m_surface);

    if (commitText.isEmpty()) {
        return {};
    }

    QString remaining;
    if (auto *candidate = m_lookupCandidates.value(index)) {
        int offset = pinyin_choose_candidate(m_instance, 0, candidate);
        QByteArray surfaceUtf8 = m_surface.toUtf8();
        if (offset <= 0 || offset > surfaceUtf8.size()) {
            return {};
        }
        remaining = QString::fromUtf8(surfaceUtf8.mid(offset));
        while (remaining.startsWith(QLatin1Char('\''))) {
            remaining.remove(0, 1);
        }
    }

    m_surface = remaining;
    refreshLookup();
    return commitText;
}

bool LibPinyinAdapter::acceptsText(const QString &text) const
{
    if (text.size() != 1) {
        return false;
    }
    if (text.at(0).isLetter()) {
        return true;
    }
    if (text == QStringLiteral("'")) {
        // Syllable separator. Double pinyin syllables always take two keys, and libpinyin's double pinyin parser stops at it.
        return !isDoublePinyin();
    }
    if (text == QStringLiteral(";")) {
        // In schemes where ';' is a final, it can only continue a syllable. It can't start one, so leave it as punctuation otherwise.
        return usesSemicolon() && !m_surface.isEmpty();
    }
    return false;
}

void LibPinyinAdapter::refreshLookup()
{
    m_lookupCandidates.clear();
    if (m_surface.isEmpty()) {
        reset();
        return;
    }

    if (!rebuildLookup()) {
        m_candidates = {m_surface};
        return;
    }
}

bool LibPinyinAdapter::rebuildLookup()
{
    pinyin_context_t *context = sharedPinyinContext();
    if (!context) {
        return false;
    }

    if (m_instance) {
        pinyin_reset(m_instance);
    } else {
        m_instance = pinyin_alloc_instance(context);
        if (!m_instance) {
            return false;
        }
    }

    const QByteArray surfaceUtf8 = m_surface.toUtf8();
    if (isDoublePinyin()) {
        // The context is shared, so always set the scheme rather than assuming what it was last set to
        if (!pinyin_set_double_pinyin_scheme(context, libpinyinDoubleScheme(m_scheme))) {
            return false;
        }
        pinyin_parse_more_double_pinyins(m_instance, surfaceUtf8.toLower().constData());
    } else {
        pinyin_parse_more_full_pinyins(m_instance, surfaceUtf8.toLower().constData());
    }
    const qsizetype parsedLength = qsizetype(pinyin_get_parsed_input_length(m_instance));
    const QString unparsedSuffix = QString::fromUtf8(surfaceUtf8.mid(parsedLength)); // Hold suffix if we are doing partial match (ex. 你hao)
    pinyin_guess_sentence(m_instance);
    if (!pinyin_guess_candidates(m_instance, 0, SORT_BY_PHRASE_LENGTH_AND_FREQUENCY)) {
        return false;
    }

    guint candidateCount = 0;
    if (!pinyin_get_n_candidate(m_instance, &candidateCount)) {
        return false;
    }

    QStringList candidateList;
    guint maxCandidates = qMin(candidateCount, CANDIDATES_LIMIT);
    for (guint i = 0; i < maxCandidates; ++i) {
        lookup_candidate_t *candidate = nullptr;
        const gchar *candidateUtf8 = nullptr;
        lookup_candidate_type_t type;
        if (!pinyin_get_candidate(m_instance, i, &candidate) || !candidate || !pinyin_get_candidate_string(m_instance, candidate, &candidateUtf8)
            || !candidateUtf8 || !pinyin_get_candidate_type(m_instance, candidate, &type)) {
            continue;
        }

        QString candidateText = QString::fromUtf8(candidateUtf8);
        if (type == NBEST_MATCH_CANDIDATE) {
            candidateText += unparsedSuffix;
            candidate = nullptr;
        }
        if (!candidateText.isEmpty()) {
            candidateList.append(candidateText);
            m_lookupCandidates.append(candidate);
        }
    }

    if (!candidateList.contains(m_surface)) {
        candidateList.append(m_surface);
        m_lookupCandidates.append(nullptr);
    }
    m_candidates = candidateList;
    return true;
}
