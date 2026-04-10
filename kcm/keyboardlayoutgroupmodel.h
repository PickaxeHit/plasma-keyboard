/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "keyboardlayoutmodel.h"

#include <QAbstractListModel>
#include <QStringList>

class KeyboardLayoutGroupModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Role {
        GroupIdRole = Qt::UserRole + 1,
        NameRole,
        DescriptionRole,
        LayoutsRole,
    };
    Q_ENUM(Role)

    struct Group {
        QString groupId;
        QString name;
        QString description;
        KeyboardLayoutModel *layouts = nullptr;
    };

    explicit KeyboardLayoutGroupModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    KeyboardLayoutModel *layouts() const;
    QString layoutForLocale(const QString &locale) const;

    void loadInstalledGroups();
    void setFormFactorFilter(const QStringList &formFactorFilter);

private:
    bool matchesFormFactor(const QStringList &formFactors) const;
    void clearGroups();

    QList<Group> m_groups;
    KeyboardLayoutModel *m_layouts = nullptr;
    QStringList m_formFactorFilter;
};
