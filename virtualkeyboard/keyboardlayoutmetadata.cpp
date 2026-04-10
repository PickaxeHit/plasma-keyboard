/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "keyboardlayoutmetadata.h"

#include <QDir>
#include <QFileInfo>
#include <QLocale>
#include <QStandardPaths>

using namespace Qt::StringLiterals;

QStringList KeyboardLayoutMetadata::rootPaths()
{
    return QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, u"plasma/keyboard/keyboardpackages"_s, QStandardPaths::LocateDirectory);
}

QVariantMap KeyboardLayoutMetadata::keyboardSection(const QVariantMap &metadata)
{
    return metadata.value(u"X-Plasma-Keyboard"_s).toMap();
}

QVariantMap KeyboardLayoutMetadata::keyboardSection(const KPluginMetaData &metadata)
{
    return keyboardSection(metadata.rawData().toVariantMap());
}

QStringList KeyboardLayoutMetadata::locales(const QVariantMap &metadata)
{
    return keyboardSection(metadata).value(u"Locales"_s).toStringList();
}

QStringList KeyboardLayoutMetadata::locales(const KPluginMetaData &metadata)
{
    return keyboardSection(metadata).value(u"Locales"_s).toStringList();
}

QString KeyboardLayoutMetadata::primaryLocale(const QVariantMap &metadata)
{
    const QStringList allLocales = locales(metadata);
    return allLocales.isEmpty() ? QString() : allLocales.constFirst();
}

QString KeyboardLayoutMetadata::localeDisplayName(const QString &locale)
{
    if (locale.isEmpty()) {
        return {};
    }

    const QLocale qlocale(locale);
    const QString language = qlocale.nativeLanguageName();
    const QString territory = qlocale.nativeTerritoryName();
    return territory.isEmpty() ? language : language + u" ("_s + territory + u")"_s;
}

QString KeyboardLayoutMetadata::packageId(const Entry &entry)
{
    return entry.metadata.pluginId().isEmpty() ? entry.id : entry.metadata.pluginId();
}

QVariantList KeyboardLayoutMetadata::keyboardLayouts(const KPluginMetaData &metadata)
{
    return keyboardSection(metadata).value(u"KeyboardLayouts"_s).toList();
}

QString KeyboardLayoutMetadata::keyboardLayoutId(const QVariantMap &layout)
{
    return layout.value(u"Id"_s).toString();
}

QString KeyboardLayoutMetadata::keyboardLayoutName(const QVariantMap &layout)
{
    return layout.value(u"Name"_s).toString();
}

QString KeyboardLayoutMetadata::keyboardLayoutFile(const QVariantMap &layout, const QString &layoutType)
{
    const QVariantMap files = layout.value(u"Files"_s).toMap();
    const QString fileName = files.value(layoutType).toString();
    if (!fileName.isEmpty()) {
        return fileName;
    }
    return layoutType + u".qml"_s;
}

QString KeyboardLayoutMetadata::fullLayoutId(const QString &packageId, const QString &layoutId)
{
    return packageId + u"/"_s + layoutId;
}

QString KeyboardLayoutMetadata::shortLayoutId(const QString &keyboardLayoutId)
{
    return keyboardLayoutId.section(QLatin1Char('/'), 1, 1);
}

QString KeyboardLayoutMetadata::keyboardLayoutDisplayName(const KeyboardLayoutEntry &entry)
{
    const QString packageName = entry.metadata.name();
    const QString layoutName = keyboardLayoutName(entry.definition);

    if (!packageName.isEmpty() && !layoutName.isEmpty()) {
        return packageName + u" - "_s + layoutName;
    }

    return packageName.isEmpty() ? entry.id : packageName;
}

QList<KeyboardLayoutMetadata::Entry> KeyboardLayoutMetadata::findInstalledLayouts()
{
    QList<Entry> layouts;

    const QStringList roots = rootPaths();
    for (const auto &root : roots) {
        const QDir rootDir(root);
        const QFileInfoList entries = rootDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const auto &entry : entries) {
            const QString metadataPath = entry.absoluteFilePath() + u"/metadata.json"_s;
            if (!QFileInfo::exists(metadataPath)) {
                continue;
            }

            const KPluginMetaData metadata = KPluginMetaData::fromJsonFile(metadataPath);
            if (!metadata.isValid()) {
                continue;
            }

            layouts.append({
                .id = entry.fileName(),
                .path = entry.absoluteFilePath(),
                .metadata = metadata,
            });
        }
    }

    return layouts;
}

QList<KeyboardLayoutMetadata::KeyboardLayoutEntry> KeyboardLayoutMetadata::keyboardLayoutEntries()
{
    QList<KeyboardLayoutEntry> layouts;

    for (const auto &entry : findInstalledLayouts()) {
        const QString currentPackageId = packageId(entry);

        const QVariantList definitions = keyboardLayouts(entry.metadata);
        for (const auto &value : definitions) {
            const QVariantMap definition = value.toMap();
            const QString currentLayoutId = keyboardLayoutId(definition);
            if (currentLayoutId.isEmpty()) {
                continue;
            }

            layouts.append({
                .id = fullLayoutId(currentPackageId, currentLayoutId),
                .packageId = currentPackageId,
                .packagePath = entry.path,
                .layoutId = currentLayoutId,
                .metadata = entry.metadata,
                .definition = definition,
            });
        }
    }

    return layouts;
}
