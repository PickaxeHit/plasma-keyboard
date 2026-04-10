/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "keyboardlayoutmodel.h"

#include <QLocale>

KeyboardLayoutModel::KeyboardLayoutModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int KeyboardLayoutModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_layouts.size();
}

QVariant KeyboardLayoutModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_layouts.size()) {
        return {};
    }

    const Layout &layout = m_layouts.at(index.row());
    switch (role) {
    case LayoutIdRole:
        return layout.layoutId;
    case NameRole:
        return layout.name;
    case DescriptionRole:
        return layout.description;
    default:
        return {};
    }
}

QHash<int, QByteArray> KeyboardLayoutModel::roleNames() const
{
    return {
        {LayoutIdRole, "layoutId"},
        {NameRole, "name"},
        {DescriptionRole, "description"},
    };
}

QVariantMap KeyboardLayoutModel::get(int row) const
{
    if (row < 0 || row >= m_layouts.size()) {
        return {};
    }

    const Layout &layout = m_layouts.at(row);
    return {
        {QStringLiteral("layoutId"), layout.layoutId},
        {QStringLiteral("name"), layout.name},
        {QStringLiteral("description"), layout.description},
    };
}

QString KeyboardLayoutModel::layoutForLocale(const QString &locale) const
{
    const QString languageCode = locale.section(QLatin1Char('_'), 0, 0);

    for (const auto &layout : m_layouts) {
        if (layout.locales.contains(locale)) {
            return layout.layoutId;
        }
    }

    for (const auto &layout : m_layouts) {
        for (const auto &candidateLocale : layout.locales) {
            if (candidateLocale.section(QLatin1Char('_'), 0, 0) == languageCode) {
                return layout.layoutId;
            }
        }
    }

    return QStringLiteral("org.kde.plasma.keyboard.en/qwerty");
}

void KeyboardLayoutModel::setLayouts(QList<Layout> layouts)
{
    beginResetModel();
    m_layouts = std::move(layouts);
    endResetModel();
    Q_EMIT countChanged();
}

QString KeyboardLayoutModel::descriptionForLocales(const QStringList &locales)
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
