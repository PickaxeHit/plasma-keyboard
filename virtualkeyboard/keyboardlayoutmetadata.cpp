/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "keyboardlayoutmetadata.h"

#include <KPluginMetaData>

#include <QDir>
#include <QFileInfo>
#include <QLocale>
#include <QStandardPaths>
#include <QVariantList>

using namespace Qt::StringLiterals;

namespace
{
QVariantMap keyboardSection(const KPluginMetaData &metadata)
{
    return metadata.rawData().toVariantMap().value(u"X-Plasma-Keyboard"_s).toMap();
}

QStringList locales(const KPluginMetaData &metadata)
{
    return keyboardSection(metadata).value(u"Locales"_s).toStringList();
}

QVariantList keyboardLayoutDefinitions(const KPluginMetaData &metadata)
{
    return keyboardSection(metadata).value(u"KeyboardLayouts"_s).toList();
}

QString packageId(const QString &directoryName, const KPluginMetaData &metadata)
{
    return metadata.pluginId().isEmpty() ? directoryName : metadata.pluginId();
}

QStringList rootPaths()
{
    return QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, u"plasma/keyboard/keyboardpackages"_s, QStandardPaths::LocateDirectory);
}

QString keyboardLayoutId(const QVariantMap &layout)
{
    return layout.value(u"Id"_s).toString();
}
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

QStringList KeyboardLayoutMetadata::keyboardLayoutFormFactors(const QVariantMap &layout)
{
    return layout.value(u"FormFactors"_s).toStringList();
}

QString KeyboardLayoutMetadata::keyboardLayoutDisplayName(const Layout &layout)
{
    const QString layoutName = keyboardLayoutName(layout.definition);

    if (!layout.packageName.isEmpty() && !layoutName.isEmpty()) {
        return layout.packageName + u" - "_s + layoutName;
    }

    return layout.packageName.isEmpty() ? layout.id : layout.packageName;
}

QList<KeyboardLayoutMetadata::Layout> KeyboardLayoutMetadata::keyboardLayouts()
{
    QList<Layout> layouts;

    for (const auto &root : rootPaths()) {
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

            const QString currentPackageId = packageId(entry.fileName(), metadata);
            for (const auto &value : keyboardLayoutDefinitions(metadata)) {
                const QVariantMap definition = value.toMap();
                const QString currentLayoutId = keyboardLayoutId(definition);
                if (currentLayoutId.isEmpty()) {
                    continue;
                }

                layouts.append({
                    .id = currentPackageId + u"/"_s + currentLayoutId,
                    .packageId = currentPackageId,
                    .packagePath = entry.absoluteFilePath(),
                    .layoutId = currentLayoutId,
                    .packageName = metadata.name(),
                    .locales = locales(metadata),
                    .formFactors = keyboardLayoutFormFactors(definition),
                    .definition = definition,
                });
            }
        }
    }

    return layouts;
}
