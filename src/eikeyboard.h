// SPDX-FileCopyrightText: 2026 Jason A. Donenfeld <Jason@zx2c4.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QList>
#include <QObject>
#include <QPair>
#include <memory>

class QSocketNotifier;
struct ei;
struct ei_device;

class EiKeyboard : public QObject
{
public:
    explicit EiKeyboard(QObject *parent = nullptr);
    ~EiKeyboard() override;

    bool initialize();
    void key(quint32 keycode, bool pressed);

private:
    void dispatch();
    void reset();

    ei *m_ei = nullptr;
    QList<ei_device *> m_keyboards;
    std::unique_ptr<QSocketNotifier> m_notifier;
    QList<QPair<quint32, bool>> m_pendingKeys;
    int m_cookie = 0;
    QString m_service;
    bool m_initializing = false;
    quint32 m_sequence = 0;
};
