/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "plasmakeyboardkcm.h"

#include "keyboardlayoutmetadata.h"

#include <QLocale>
#include <QVariantMap>
#include <algorithm>

K_PLUGIN_CLASS_WITH_JSON(PlasmaKeyboardKcm, "kcm_plasmakeyboard.json")

PlasmaKeyboardKcm::PlasmaKeyboardKcm(QObject *parent, const KPluginMetaData &metaData)
    : KQuickManagedConfigModule(parent, metaData)
{
    // clang-format off
    qmlRegisterSingletonInstance<PlasmaKeyboardSettings>(
        "org.kde.plasma.keyboard.settings",
        1,
        0,
        "PlasmaKeyboardSettings",
        PlasmaKeyboardSettings::self()
    );
    // clang-format on

    loadAvailableKeyboardLayouts();
    load();
}

bool PlasmaKeyboardKcm::soundEnabled() const
{
    return m_soundEnabled;
}

void PlasmaKeyboardKcm::setSoundEnabled(bool soundEnabled)
{
    if (soundEnabled == m_soundEnabled) {
        return;
    }

    m_soundEnabled = soundEnabled;
    Q_EMIT soundEnabledChanged();

    setNeedsSave(true);
}

bool PlasmaKeyboardKcm::vibrationEnabled() const
{
    return m_vibrationEnabled;
}

void PlasmaKeyboardKcm::setVibrationEnabled(bool vibrationEnabled)
{
    if (vibrationEnabled == m_vibrationEnabled) {
        return;
    }

    m_vibrationEnabled = vibrationEnabled;
    Q_EMIT vibrationEnabledChanged();

    setNeedsSave(true);
}

QStringList PlasmaKeyboardKcm::enabledKeyboardLayoutIds() const
{
    return m_enabledKeyboardLayoutIds;
}

QVariantList PlasmaKeyboardKcm::availableKeyboardLayouts() const
{
    return m_availableKeyboardLayouts;
}

void PlasmaKeyboardKcm::enableKeyboardLayout(const QString &layoutId)
{
    if (m_enabledKeyboardLayoutIds.contains(layoutId)) {
        return;
    }

    m_enabledKeyboardLayoutIds.append(layoutId);
    Q_EMIT enabledKeyboardLayoutIdsChanged();

    setNeedsSave(true);
}

void PlasmaKeyboardKcm::disableKeyboardLayout(const QString &layoutId)
{
    if (!m_enabledKeyboardLayoutIds.contains(layoutId)) {
        return;
    }

    m_enabledKeyboardLayoutIds.removeAll(layoutId);
    Q_EMIT enabledKeyboardLayoutIdsChanged();

    setNeedsSave(true);
}

bool PlasmaKeyboardKcm::keyboardNavigationEnabled() const
{
    return m_keyboardNavigationEnabled;
}

void PlasmaKeyboardKcm::setKeyboardNavigationEnabled(bool keyboardNavigationEnabled)
{
    if (keyboardNavigationEnabled == m_keyboardNavigationEnabled) {
        return;
    }

    m_keyboardNavigationEnabled = keyboardNavigationEnabled;
    Q_EMIT keyboardNavigationEnabledChanged();

    setNeedsSave(true);
}

bool PlasmaKeyboardKcm::autoCapitalizationEnabled() const
{
    return m_autoCapitalizationEnabled;
}

void PlasmaKeyboardKcm::setAutoCapitalizationEnabled(bool autoCapitalizationEnabled)
{
    if (autoCapitalizationEnabled == m_autoCapitalizationEnabled) {
        return;
    }

    m_autoCapitalizationEnabled = autoCapitalizationEnabled;
    Q_EMIT autoCapitalizationEnabledChanged();

    setNeedsSave(true);
}

bool PlasmaKeyboardKcm::diacriticsPopupEnabled() const
{
    return m_diacriticsPopupEnabled;
}

void PlasmaKeyboardKcm::setDiacriticsPopupEnabled(bool enabled)
{
    if (enabled == m_diacriticsPopupEnabled) {
        return;
    }

    m_diacriticsPopupEnabled = enabled;
    Q_EMIT diacriticsPopupEnabledChanged();

    setNeedsSave(true);
}

int PlasmaKeyboardKcm::diacriticsHoldThresholdMs() const
{
    return m_diacriticsHoldThresholdMs;
}

void PlasmaKeyboardKcm::setDiacriticsHoldThresholdMs(int thresholdMs)
{
    if (thresholdMs == m_diacriticsHoldThresholdMs) {
        return;
    }

    m_diacriticsHoldThresholdMs = thresholdMs;
    Q_EMIT diacriticsHoldThresholdMsChanged();

    setNeedsSave(true);
}

bool PlasmaKeyboardKcm::isSaveNeeded() const
{
    return m_saveNeeded;
}

void PlasmaKeyboardKcm::load()
{
    setSoundEnabled(PlasmaKeyboardSettings::self()->soundEnabled());
    setVibrationEnabled(PlasmaKeyboardSettings::self()->vibrationEnabled());

    m_enabledKeyboardLayoutIds = PlasmaKeyboardSettings::self()->enabledKeyboardLayoutIds();
    if (m_enabledKeyboardLayoutIds.isEmpty()) {
        const QStringList enabledLocales = PlasmaKeyboardSettings::self()->enabledLocales();
        for (const auto &locale : enabledLocales) {
            const QString layoutId = layoutForLocale(locale);
            if (!layoutId.isEmpty() && !m_enabledKeyboardLayoutIds.contains(layoutId)) {
                m_enabledKeyboardLayoutIds.append(layoutId);
            }
        }
    }
    Q_EMIT enabledKeyboardLayoutIdsChanged();
    setKeyboardNavigationEnabled(PlasmaKeyboardSettings::self()->keyboardNavigationEnabled());
    setAutoCapitalizationEnabled(PlasmaKeyboardSettings::self()->autoCapitalizationEnabled());
    setDiacriticsPopupEnabled(PlasmaKeyboardSettings::self()->diacriticsPopupEnabled());
    setDiacriticsHoldThresholdMs(PlasmaKeyboardSettings::self()->diacriticsHoldThresholdMs());

    setNeedsSave(false);
}

void PlasmaKeyboardKcm::save()
{
    PlasmaKeyboardSettings::self()->setSoundEnabled(m_soundEnabled);
    PlasmaKeyboardSettings::self()->setVibrationEnabled(m_vibrationEnabled);
    PlasmaKeyboardSettings::self()->setEnabledKeyboardLayoutIds(m_enabledKeyboardLayoutIds);
    PlasmaKeyboardSettings::self()->setEnabledLocales({});
    PlasmaKeyboardSettings::self()->setKeyboardNavigationEnabled(m_keyboardNavigationEnabled);
    PlasmaKeyboardSettings::self()->setAutoCapitalizationEnabled(m_autoCapitalizationEnabled);
    PlasmaKeyboardSettings::self()->setDiacriticsPopupEnabled(m_diacriticsPopupEnabled);
    PlasmaKeyboardSettings::self()->setDiacriticsHoldThresholdMs(m_diacriticsHoldThresholdMs);
    PlasmaKeyboardSettings::self()->save();

    setNeedsSave(false);
}

void PlasmaKeyboardKcm::loadAvailableKeyboardLayouts()
{
    QVariantList layouts;

    for (const auto &entry : KeyboardLayoutMetadata::keyboardLayoutEntries()) {
        const QStringList locales = KeyboardLayoutMetadata::locales(entry.metadata);
        QVariantMap layout;
        layout.insert(QStringLiteral("id"), entry.id);
        layout.insert(QStringLiteral("name"), KeyboardLayoutMetadata::keyboardLayoutDisplayName(entry));
        layout.insert(QStringLiteral("description"), layoutDescription(locales));
        layout.insert(QStringLiteral("locales"), locales);
        layouts.append(layout);
    }

    std::sort(layouts.begin(), layouts.end(), [](const QVariant &left, const QVariant &right) {
        return left.toMap().value(QStringLiteral("name")).toString().localeAwareCompare(right.toMap().value(QStringLiteral("name")).toString()) < 0;
    });

    m_availableKeyboardLayouts = layouts;
    Q_EMIT availableKeyboardLayoutsChanged();
}

QString PlasmaKeyboardKcm::layoutForLocale(const QString &locale) const
{
    const QString languageCode = locale.section(QLatin1Char('_'), 0, 0);

    for (const auto &entry : m_availableKeyboardLayouts) {
        const QVariantMap layout = entry.toMap();
        const QStringList locales = layout.value(QStringLiteral("locales")).toStringList();
        if (locales.contains(locale)) {
            return layout.value(QStringLiteral("id")).toString();
        }
    }

    for (const auto &entry : m_availableKeyboardLayouts) {
        const QVariantMap layout = entry.toMap();
        const QStringList locales = layout.value(QStringLiteral("locales")).toStringList();
        for (const auto &candidateLocale : locales) {
            if (candidateLocale.section(QLatin1Char('_'), 0, 0) == languageCode) {
                return layout.value(QStringLiteral("id")).toString();
            }
        }
    }

    return QStringLiteral("org.kde.plasma.keyboard.en/qwerty");
}

QString PlasmaKeyboardKcm::layoutDescription(const QStringList &locales) const
{
    QStringList names;
    names.reserve(locales.size());

    for (const auto &locale : locales) {
        const QLocale qlocale(locale);
        const QString language = qlocale.nativeLanguageName();
        const QString territory = qlocale.nativeTerritoryName();
        names.append(territory.isEmpty() ? language : language + QStringLiteral(" (") + territory + QStringLiteral(")"));
    }

    return names.join(QStringLiteral(", "));
}

#include "plasmakeyboardkcm.moc"

#include "moc_plasmakeyboardkcm.cpp"
