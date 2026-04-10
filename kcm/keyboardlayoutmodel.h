/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QAbstractListModel>
#include <QStringList>
#include <QVariantMap>

class KeyboardLayoutModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    enum Role {
        LayoutIdRole = Qt::UserRole + 1,
        NameRole,
        DescriptionRole,
    };
    Q_ENUM(Role)

    struct Layout {
        QString layoutId;
        QString name;
        QString description;
        QStringList locales;
    };

    explicit KeyboardLayoutModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE QVariantMap get(int row) const;

    QString layoutForLocale(const QString &locale) const;
    void setLayouts(QList<Layout> layouts);
    static QString descriptionForLocales(const QStringList &locales);

Q_SIGNALS:
    void countChanged();

private:
    QList<Layout> m_layouts;
};
