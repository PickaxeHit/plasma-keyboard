/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "textexpansiontrigger.h"

#include "logging.h"
#include "overlaycontroller.h"

#include <KLocalizedString>
#include <qdatetime.h>

TextExpansionTrigger::TextExpansionTrigger(QObject *parent)
    : OverlayTrigger(parent)
{
    // POC: hardcoded expansions; TODO: load from settings
    addExpansion(QStringLiteral(":email"), QStringLiteral("my_email@mydomain.com"));
    addExpansion(QStringLiteral(":hi"), QStringLiteral("Heya! o/"));
    addExpansion(QStringLiteral(":date"), QDate::currentDate().toString(Qt::ISODate));
}

QString TextExpansionTrigger::triggerId() const
{
    return QStringLiteral("textexpansion");
}

QString TextExpansionTrigger::displayName() const
{
    return i18nc("@label Name of the text expansion overlay trigger", "Text Expansion");
}

// clang-format off
OverlayTriggerResult TextExpansionTrigger::processEvent(OverlayInputEvent eventType,
                                                              const QKeyEvent *keyEvent,
                                                              const QString &text,
                                                              OverlayController *controller)
// clang-format on
{
    Q_UNUSED(keyEvent)
    Q_UNUSED(controller)

    OverlayTriggerResult result;

    switch (eventType) {
    case OverlayInputEvent::TextBeforeCursorChanged: {
        // `text` is the text before the cursor.
        const QString abbrev = findMatchingAbbreviation(text);
        if (!abbrev.isEmpty()) {
            result.action = OverlayAction::ReplaceText;
            result.deleteBeforeCursor = abbrev.length();
            result.commitText = m_expansions.value(abbrev);
        }
        break;
    }

    case OverlayInputEvent::KeyPress:
    case OverlayInputEvent::KeyRelease:
    case OverlayInputEvent::PreeditChanged:
    case OverlayInputEvent::TimerExpired:
        // Not used
        break;
    }

    return result;
}

void TextExpansionTrigger::reset()
{
}

bool TextExpansionTrigger::isEnabled() const
{
    // TODO: Add setting for text expansion
    // return PlasmaKeyboardSettings::self()->textExpansionEnabled();
    return true; // POC: always on
}

QStringList TextExpansionTrigger::candidates(const QString &baseText) const
{
    // Text expansion doesn't show candidates in a popup (it replaces directly)
    // But we could show matching expansions if we wanted preview
    Q_UNUSED(baseText)
    return {};
}

void TextExpansionTrigger::addExpansion(const QString &abbreviation, const QString &expansion)
{
    m_expansions.insert(abbreviation, expansion);
}

void TextExpansionTrigger::removeExpansion(const QString &abbreviation)
{
    m_expansions.remove(abbreviation);
}

QString TextExpansionTrigger::findMatchingAbbreviation(const QString &text) const
{
    // Check if text ends with any known abbreviation
    // We check longest matches first for proper precedence
    QList<QString> abbrevs = m_expansions.keys();
    std::sort(abbrevs.begin(), abbrevs.end(), [](const QString &a, const QString &b) {
        return a.length() > b.length();
    });

    for (const auto &abbrev : abbrevs) {
        if (text.endsWith(abbrev)) {
            // Ensure it's a word boundary (start of string or preceded by space)
            const int pos = text.length() - abbrev.length();
            if (pos == 0 || text.at(pos - 1).isSpace()) {
                return abbrev;
            }
        }
    }

    return {};
}

#include "moc_textexpansiontrigger.cpp"
