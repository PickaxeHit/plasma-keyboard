/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "keyboardpackageresolver.h"
#include "keyboardlayoutmetadata.h"
#include "logging.h"

#include <QFileInfo>

using namespace Qt::StringLiterals;

KeyboardPackageResolver::KeyboardPackageResolver(QObject *parent)
    : QObject(parent)
{
    rebuildIndex();
}

QStringList KeyboardPackageResolver::keyboardLayoutIds() const
{
    return m_keyboardLayoutIds;
}

QString KeyboardPackageResolver::packageId(const QString &keyboardLayoutId) const
{
    const int index = indexOfKeyboardLayout(resolveKeyboardLayoutId(keyboardLayoutId));
    if (index < 0) {
        return {};
    }
    return m_keyboardLayouts.at(index).packageId;
}

QString KeyboardPackageResolver::resolveKeyboardLayoutId(const QString &keyboardLayoutId) const
{
    for (const auto &layout : m_keyboardLayouts) {
        if (layout.id == keyboardLayoutId) {
            return layout.id;
        }
    }

    for (const auto &layout : m_keyboardLayouts) {
        if (layout.packageId == keyboardLayoutId) {
            return layout.id;
        }
    }

    return {};
}

QString KeyboardPackageResolver::keyboardLayoutName(const QString &keyboardLayoutId) const
{
    const int index = indexOfKeyboardLayout(resolveKeyboardLayoutId(keyboardLayoutId));
    if (index < 0) {
        return {};
    }

    KeyboardLayoutMetadata::KeyboardLayoutEntry entry{
        .id = m_keyboardLayouts.at(index).id,
        .packageId = m_keyboardLayouts.at(index).packageId,
        .packagePath = m_keyboardLayouts.at(index).path,
        .layoutId = KeyboardLayoutMetadata::shortLayoutId(m_keyboardLayouts.at(index).id),
        .metadata = m_keyboardLayouts.at(index).metadata,
        .definition = m_keyboardLayouts.at(index).definition,
    };
    return KeyboardLayoutMetadata::keyboardLayoutDisplayName(entry);
}

QVariantMap KeyboardPackageResolver::keyboardLayoutMetadata(const QString &keyboardLayoutId) const
{
    const int index = indexOfKeyboardLayout(resolveKeyboardLayoutId(keyboardLayoutId));
    if (index < 0) {
        return {};
    }

    QVariantMap metadata = m_keyboardLayouts.at(index).metadata.rawData().toVariantMap();
    metadata.insert(u"X-Plasma-Keyboard-Layout"_s, m_keyboardLayouts.at(index).definition);
    return metadata;
}

QString KeyboardPackageResolver::packagePath(const QString &keyboardLayoutId) const
{
    const int index = indexOfKeyboardLayout(resolveKeyboardLayoutId(keyboardLayoutId));
    if (index < 0) {
        return {};
    }
    return m_keyboardLayouts.at(index).path;
}

QString KeyboardPackageResolver::layoutPath(const QString &keyboardLayoutId, const QString &layoutType) const
{
    const int index = indexOfKeyboardLayout(resolveKeyboardLayoutId(keyboardLayoutId));
    if (index < 0 || layoutType.isEmpty()) {
        return {};
    }

    const QString fileName = KeyboardLayoutMetadata::keyboardLayoutFile(m_keyboardLayouts.at(index).definition, layoutType);
    const QString layoutPath = m_keyboardLayouts.at(index).path + u"/contents/layouts/"_s + fileName;
    if (QFileInfo::exists(layoutPath)) {
        return layoutPath;
    }
    return {};
}

QUrl KeyboardPackageResolver::layoutUrl(const QString &keyboardLayoutId, const QString &layoutType) const
{
    const QString path = layoutPath(keyboardLayoutId, layoutType);
    qCWarning(PlasmaKeyboard) << "KeyboardPackageResolver: loading keyboard layout" << keyboardLayoutId << layoutType << path;
    return path.isEmpty() ? QUrl() : QUrl::fromLocalFile(path);
}

void KeyboardPackageResolver::rebuildIndex()
{
    QList<KeyboardLayoutEntry> keyboardLayouts;

    const QStringList roots = KeyboardLayoutMetadata::rootPaths();
    qCWarning(PlasmaKeyboard) << "KeyboardPackageResolver: scanning keyboard layout roots" << roots;

    const QList<KeyboardLayoutMetadata::KeyboardLayoutEntry> discoveredLayouts = KeyboardLayoutMetadata::keyboardLayoutEntries();
    for (const auto &layout : discoveredLayouts) {
        qCWarning(PlasmaKeyboard) << "KeyboardPackageResolver: discovered keyboard layout" << layout.id << layout.packageId << layout.packagePath;

        keyboardLayouts.append({
            .id = layout.id,
            .packageId = layout.packageId,
            .metadata = layout.metadata,
            .path = layout.packagePath,
            .definition = layout.definition,
        });
    }

    QStringList ids;
    ids.reserve(keyboardLayouts.size());
    for (const auto &keyboardLayout : keyboardLayouts) {
        ids.append(keyboardLayout.id);
    }

    if (m_keyboardLayoutIds == ids && m_keyboardLayouts.size() == keyboardLayouts.size()) {
        return;
    }

    m_keyboardLayouts = keyboardLayouts;
    m_keyboardLayoutIds = ids;
    qCWarning(PlasmaKeyboard) << "KeyboardPackageResolver: active keyboard layouts" << m_keyboardLayoutIds;
    Q_EMIT keyboardLayoutIdsChanged();
}

int KeyboardPackageResolver::indexOfKeyboardLayout(const QString &keyboardLayoutId) const
{
    for (int i = 0; i < m_keyboardLayouts.size(); ++i) {
        if (m_keyboardLayouts.at(i).id == keyboardLayoutId) {
            return i;
        }
    }
    return -1;
}
