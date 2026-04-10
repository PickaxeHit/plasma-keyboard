/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QObject>
#include <QStringList>
#include <QUrl>
#include <QVariantMap>
#include <qqmlintegration.h>

#include <KPluginMetaData>

class KeyboardPackageResolver : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS
    Q_PROPERTY(QStringList keyboardLayoutIds READ keyboardLayoutIds NOTIFY keyboardLayoutIdsChanged)

public:
    explicit KeyboardPackageResolver(QObject *parent = nullptr);

    QStringList keyboardLayoutIds() const;

    Q_INVOKABLE QString packageId(const QString &keyboardLayoutId) const;
    Q_INVOKABLE QString resolveKeyboardLayoutId(const QString &keyboardLayoutId) const;
    Q_INVOKABLE QString keyboardLayoutName(const QString &keyboardLayoutId) const;
    Q_INVOKABLE QVariantMap keyboardLayoutMetadata(const QString &keyboardLayoutId) const;
    Q_INVOKABLE QString packagePath(const QString &keyboardLayoutId) const;
    Q_INVOKABLE QString layoutPath(const QString &keyboardLayoutId, const QString &layoutType) const;
    Q_INVOKABLE QUrl layoutUrl(const QString &keyboardLayoutId, const QString &layoutType) const;

Q_SIGNALS:
    void keyboardLayoutIdsChanged();

private:
    struct KeyboardLayoutEntry {
        QString id;
        QString packageId;
        KPluginMetaData metadata;
        QString path;
        QVariantMap definition;
    };

    void rebuildIndex();
    int indexOfKeyboardLayout(const QString &keyboardLayoutId) const;

    QStringList m_keyboardLayoutIds;
    QList<KeyboardLayoutEntry> m_keyboardLayouts;
};
