/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "keyboardlayoutgroupmodel.h"

#include "keyboardlayoutmetadata.h"

#include <QMap>
#include <algorithm>
#include <utility>

KeyboardLayoutGroupModel::KeyboardLayoutGroupModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_layouts(new KeyboardLayoutModel(this))
{
}

int KeyboardLayoutGroupModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_groups.size();
}

QVariant KeyboardLayoutGroupModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_groups.size()) {
        return {};
    }

    const Group &group = m_groups.at(index.row());
    switch (role) {
    case GroupIdRole:
        return group.groupId;
    case NameRole:
        return group.name;
    case DescriptionRole:
        return group.description;
    case LayoutsRole:
        return QVariant::fromValue(group.layouts);
    default:
        return {};
    }
}

QHash<int, QByteArray> KeyboardLayoutGroupModel::roleNames() const
{
    return {
        {GroupIdRole, "groupId"},
        {NameRole, "name"},
        {DescriptionRole, "description"},
        {LayoutsRole, "layouts"},
    };
}

QAbstractItemModel *KeyboardLayoutGroupModel::layouts() const
{
    return m_layouts;
}

QString KeyboardLayoutGroupModel::layoutForLocale(const QString &locale) const
{
    return m_layouts->layoutForLocale(locale);
}

void KeyboardLayoutGroupModel::setFormFactorFilter(const QStringList &formFactorFilter)
{
    if (m_formFactorFilter == formFactorFilter) {
        return;
    }

    m_formFactorFilter = formFactorFilter;
    loadInstalledGroups();
}

void KeyboardLayoutGroupModel::loadInstalledGroups()
{
    struct PendingGroup {
        QString groupId;
        QString name;
        QString description;
        QList<KeyboardLayoutModel::Layout> layouts;
    };

    QMap<QString, PendingGroup> groupByPackageId;
    QList<KeyboardLayoutModel::Layout> flatLayouts;

    for (const auto &entry : KeyboardLayoutMetadata::keyboardLayouts()) {
        if (!matchesFormFactor(entry.formFactors)) {
            continue;
        }

        const QString layoutName = KeyboardLayoutMetadata::keyboardLayoutName(entry.definition);
        const QString displayName = KeyboardLayoutMetadata::keyboardLayoutDisplayName(entry);

        flatLayouts.append({
            .layoutId = entry.id,
            .name = displayName,
            .description = KeyboardLayoutModel::descriptionForLocales(entry.locales),
            .locales = entry.locales,
        });

        PendingGroup group = groupByPackageId.value(entry.packageId);
        if (group.groupId.isEmpty()) {
            const QString localeDescription = KeyboardLayoutModel::descriptionForLocales(entry.locales);
            group.groupId = entry.packageId;
            group.name = entry.packageName.isEmpty() ? localeDescription : entry.packageName;
            group.description = localeDescription;
        }

        group.layouts.append({
            .layoutId = entry.id,
            .name = layoutName.isEmpty() ? displayName : layoutName,
            .description = entry.id,
            .locales = entry.locales,
        });
        groupByPackageId.insert(entry.packageId, group);
    }

    QList<Group> groups;
    const QList<PendingGroup> pendingGroups = groupByPackageId.values();
    groups.reserve(pendingGroups.size());
    for (auto group : pendingGroups) {
        std::sort(group.layouts.begin(), group.layouts.end(), [](const auto &left, const auto &right) {
            return left.name.localeAwareCompare(right.name) < 0;
        });

        auto *layoutModel = new KeyboardLayoutModel;
        layoutModel->setLayouts(std::move(group.layouts));

        groups.append({
            .groupId = group.groupId,
            .name = group.name,
            .description = group.description,
            .layouts = layoutModel,
        });
    }
    std::sort(groups.begin(), groups.end(), [](const auto &left, const auto &right) {
        return left.name.localeAwareCompare(right.name) < 0;
    });

    std::sort(flatLayouts.begin(), flatLayouts.end(), [](const auto &left, const auto &right) {
        return left.name.localeAwareCompare(right.name) < 0;
    });
    m_layouts->setLayouts(std::move(flatLayouts));

    setGroups(std::move(groups));
}

void KeyboardLayoutGroupModel::setGroups(QList<Group> groups)
{
    beginResetModel();

    for (const auto &group : std::as_const(m_groups)) {
        delete group.layouts;
    }

    m_groups = std::move(groups);
    for (const auto &group : m_groups) {
        if (group.layouts) {
            group.layouts->setParent(this);
        }
    }
    endResetModel();
}

bool KeyboardLayoutGroupModel::matchesFormFactor(const QStringList &formFactors) const
{
    if (m_formFactorFilter.isEmpty() || formFactors.isEmpty()) {
        return true;
    }

    for (const QString &formFactor : m_formFactorFilter) {
        if (formFactors.contains(formFactor)) {
            return true;
        }
    }

    return false;
}
