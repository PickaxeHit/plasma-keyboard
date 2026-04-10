/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <KPluginMetaData>

#include <QStringList>
#include <QVariantMap>

class KeyboardLayoutMetadata
{
public:
    struct Entry {
        QString id;
        QString path;
        KPluginMetaData metadata;
    };

    struct KeyboardLayoutEntry {
        QString id;
        QString packageId;
        QString packagePath;
        QString layoutId;
        KPluginMetaData metadata;
        QVariantMap definition;
    };

    static QStringList rootPaths();
    static QVariantMap keyboardSection(const QVariantMap &metadata);
    static QVariantMap keyboardSection(const KPluginMetaData &metadata);
    static QStringList locales(const QVariantMap &metadata);
    static QStringList locales(const KPluginMetaData &metadata);
    static QString primaryLocale(const QVariantMap &metadata);
    static QString localeDisplayName(const QString &locale);
    static QString packageId(const Entry &entry);
    static QVariantList keyboardLayouts(const KPluginMetaData &metadata);
    static QString keyboardLayoutId(const QVariantMap &layout);
    static QString keyboardLayoutName(const QVariantMap &layout);
    static QString keyboardLayoutFile(const QVariantMap &layout, const QString &layoutType);
    static QString fullLayoutId(const QString &packageId, const QString &layoutId);
    static QString shortLayoutId(const QString &keyboardLayoutId);
    static QString keyboardLayoutDisplayName(const KeyboardLayoutEntry &entry);
    static QList<Entry> findInstalledLayouts();
    static QList<KeyboardLayoutEntry> keyboardLayoutEntries();
};
