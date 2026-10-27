// SPDX-FileCopyrightText: 2026 Jason A. Donenfeld <Jason@zx2c4.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "eikeyboard.h"
#include "logging.h"

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QDBusPendingReply>
#include <QDBusUnixFileDescriptor>
#include <QSocketNotifier>

#include <libei.h>
#include <utility>

static QDBusMessage eisCall(const QString &method, const QString &service)
{
    return QDBusMessage::createMethodCall(service, QStringLiteral("/org/kde/KWin/EIS/RemoteDesktop"), QStringLiteral("org.kde.KWin.EIS.RemoteDesktop"), method);
}

EiKeyboard::EiKeyboard(QObject *parent)
    : QObject(parent)
{
}

EiKeyboard::~EiKeyboard()
{
    reset();
}

void EiKeyboard::reset()
{
    m_notifier.reset();
    for (auto device : std::as_const(m_keyboards)) {
        ei_device_unref(device);
    }
    m_keyboards.clear();
    m_pendingKeys.clear();
    m_initializing = false;
    m_ei = ei_unref(m_ei);
    if (m_cookie) {
        auto message = eisCall(QStringLiteral("disconnect"), m_service);
        message.setArguments({m_cookie});
        QDBusConnection::sessionBus().send(message);
        m_cookie = 0;
    }
}

bool EiKeyboard::initialize()
{
    if (m_ei) {
        return true;
    }
    auto busInterface = QDBusConnection::sessionBus().interface();
    m_service = busInterface ? busInterface->serviceOwner(QStringLiteral("org.kde.KWin")).value() : QString();
    if (m_service.isEmpty()) {
        qCWarning(PlasmaKeyboard) << "KWin is unavailable on the session bus";
        return false;
    }
    auto message = eisCall(QStringLiteral("connectToEIS"), m_service);
    message.setArguments({1}); // Keyboard
    QDBusPendingReply<QDBusUnixFileDescriptor, int> reply = QDBusConnection::sessionBus().call(message);
    if (reply.isError()) {
        qCWarning(PlasmaKeyboard) << "Failed to connect to KWin EIS:" << reply.error();
        return false;
    }
    m_cookie = reply.argumentAt<1>();
    m_ei = ei_new_sender(this);
    if (!m_ei) {
        reset();
        return false;
    }
    ei_configure_name(m_ei, QCoreApplication::applicationName().toUtf8().constData());
    if (const int error = ei_setup_backend_fd(m_ei, reply.argumentAt<0>().takeFileDescriptor()); error < 0) {
        qCWarning(PlasmaKeyboard) << "Failed to set up KWin EI:" << error;
        reset();
        return false;
    }
    m_initializing = true;
    m_notifier = std::make_unique<QSocketNotifier>(ei_get_fd(m_ei), QSocketNotifier::Read);
    connect(m_notifier.get(), &QSocketNotifier::activated, this, &EiKeyboard::dispatch);
    dispatch();
    return m_ei;
}

void EiKeyboard::key(quint32 keycode, bool pressed)
{
    dispatch();
    if (!m_ei) {
        return;
    }
    if (!m_keyboards.isEmpty()) {
        auto device = m_keyboards.first();
        ei_device_keyboard_key(device, keycode, pressed);
        ei_device_frame(device, ei_now(m_ei));
    } else if (m_initializing) {
        // Preserve the first key press/release while the EI handshake completes.
        if (m_pendingKeys.size() >= 256) {
            qCWarning(PlasmaKeyboard) << "KWin EI keyboard did not become ready";
            reset();
            return;
        }
        m_pendingKeys.append({keycode, pressed});
    }
}

void EiKeyboard::dispatch()
{
    if (!m_ei) {
        return;
    }
    ei_dispatch(m_ei);
    while (auto event = ei_get_event(m_ei)) {
        auto device = ei_event_get_device(event);
        switch (ei_event_get_type(event)) {
        case EI_EVENT_SEAT_ADDED:
            ei_seat_bind_capabilities(ei_event_get_seat(event), EI_DEVICE_CAP_KEYBOARD, nullptr);
            break;
        case EI_EVENT_DEVICE_RESUMED:
            if (ei_device_has_capability(device, EI_DEVICE_CAP_KEYBOARD) && !m_keyboards.contains(device)) {
                ei_device_start_emulating(device, ++m_sequence);
                m_keyboards.append(ei_device_ref(device));
                m_initializing = false;
                const auto pending = std::exchange(m_pendingKeys, {});
                for (const auto &[keycode, pressed] : pending) {
                    ei_device_keyboard_key(device, keycode, pressed);
                    ei_device_frame(device, ei_now(m_ei));
                }
            }
            break;
        case EI_EVENT_DEVICE_PAUSED:
        case EI_EVENT_DEVICE_REMOVED:
            if (m_keyboards.removeOne(device)) {
                ei_device_unref(device);
            }
            break;
        case EI_EVENT_DISCONNECT:
            qCWarning(PlasmaKeyboard) << "Disconnected from KWin EIS";
            ei_event_unref(event);
            reset();
            return;
        default:
            break;
        }
        ei_event_unref(event);
    }
}
