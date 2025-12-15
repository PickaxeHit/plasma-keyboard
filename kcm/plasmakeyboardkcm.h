/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <KQuickManagedConfigModule>

#include "plasmakeyboardsettings.h"

class PlasmaKeyboardKcm : public KQuickManagedConfigModule
{
    Q_OBJECT
    Q_PROPERTY(bool soundEnabled READ soundEnabled WRITE setSoundEnabled NOTIFY soundEnabledChanged)
    Q_PROPERTY(bool vibrationEnabled READ vibrationEnabled WRITE setVibrationEnabled NOTIFY vibrationEnabledChanged)
    Q_PROPERTY(QStringList enabledLocales READ enabledLocales NOTIFY enabledLocalesChanged)
    Q_PROPERTY(bool keyboardNavigationEnabled READ keyboardNavigationEnabled WRITE setKeyboardNavigationEnabled NOTIFY keyboardNavigationEnabledChanged)
    Q_PROPERTY(bool splitKeyboardEnabled READ splitKeyboardEnabled WRITE setSplitKeyboardEnabled NOTIFY splitKeyboardEnabledChanged)

public:
    PlasmaKeyboardKcm(QObject *parent, const KPluginMetaData &metaData);

    bool soundEnabled() const;
    void setSoundEnabled(bool soundEnabled);

    bool vibrationEnabled() const;
    void setVibrationEnabled(bool vibrationEnabled);

    QStringList enabledLocales() const;

    Q_INVOKABLE void enableLocale(const QString &locale);
    Q_INVOKABLE void disableLocale(const QString &locale);

    bool keyboardNavigationEnabled() const;
    void setKeyboardNavigationEnabled(bool keyboardNavigationEnabled);

    bool splitKeyboardEnabled() const;
    void setSplitKeyboardEnabled(bool splitKeyboardEnabled);

    bool isSaveNeeded() const override;

public Q_SLOTS:
    void load() override;
    void save() override;

Q_SIGNALS:
    void soundEnabledChanged();
    void vibrationEnabledChanged();
    void enabledLocalesChanged();
    void keyboardNavigationEnabledChanged();
    void splitKeyboardEnabledChanged();

private:
    bool m_soundEnabled = false;
    bool m_vibrationEnabled = true;
    bool m_keyboardNavigationEnabled = false;

    bool m_saveNeeded = false;

    QStringList m_enabledLocales;

    PlasmaKeyboardSettings *m_settings = nullptr;

    bool m_splitKeyboardEnabled = false;
};
