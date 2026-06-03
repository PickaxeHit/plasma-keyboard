/*
    SPDX-FileCopyrightText: 2021 Aleix Pol Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "inputmethod_p.h"
#include "logging.h"
#include <QDateTime>
#include <QDebug>
#include <QGuiApplication>
#include <QInputMethod>
#include <QKeyEvent>
#include <QStandardPaths>

#include <KConfigGroup>
#include <KSharedConfig>

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

InputMethod::InputMethod()
    : QWaylandClientExtensionTemplate<InputMethod>(1)
{
}

InputMethod::~InputMethod() = default;

void InputMethod::zwp_input_method_v1_activate(struct ::zwp_input_method_context_v1 *id)
{
    setCurrentContext(new InputMethodContext(id));
}

void InputMethod::zwp_input_method_v1_deactivate(struct ::zwp_input_method_context_v1 *context)
{
    Q_ASSERT(m_current->object() == context);
    setCurrentContext(nullptr);
}

void InputMethod::setCurrentContext(InputMethodContext *context)
{
    if (m_current.get() == context) {
        return;
    }
    m_current.reset(context);
    Q_EMIT activityChanged(m_current.use_count());

    if (m_current)
        Q_EMIT activate();
    else
        Q_EMIT deactivate();
}

InputMethodContext::InputMethodContext(struct ::zwp_input_method_context_v1 *id)
    : QtWayland::zwp_input_method_context_v1(id)
{
}

InputMethodContext::~InputMethodContext() = default;

void InputMethodContext::zwp_input_method_context_v1_reset()
{
    Q_EMIT reset();
}

void InputMethodContext::zwp_input_method_context_v1_commit_state(uint32_t serial)
{
    m_latestSerial = serial;
    Q_EMIT receivedCommit();
}

void InputMethodContext::zwp_input_method_context_v1_content_type(uint32_t hint, uint32_t purpose)
{
    m_contentHint = InputPlugin::ContentHint(hint);
    m_contentPurpose = InputPlugin::ContentPurpose(purpose);
    Q_EMIT contentTypeChanged();
}

void InputMethodContext::zwp_input_method_context_v1_preferred_language(const QString &language)
{
    Q_EMIT preferredLanguageChanged(language);
}

void InputMethodContext::zwp_input_method_context_v1_surrounding_text(const QString &text, uint32_t cursor, uint32_t anchor)
{
    m_text = text;
    m_cursor = cursor;
    m_anchor = anchor;
    Q_EMIT surroundingTextChanged(text, cursor, anchor);
}

void InputMethodContext::zwp_input_method_context_v1_invoke_action(uint32_t button, uint32_t index)
{
}

std::shared_ptr<Keyboard> InputMethodContext::keyboard()
{
    if (auto existing = m_keyboard.lock()) {
        return existing;
    }

    auto wlKeyboard = grab_keyboard();
    auto keyboard = std::make_shared<Keyboard>(wlKeyboard, this);
    m_keyboard = keyboard;
    return keyboard;
}

Keyboard::Keyboard(::wl_keyboard *keyboard, InputMethodContext *parent)
    : wl_keyboard(keyboard)
    , m_parent(parent)
{
    mXkbContext.reset(xkb_context_new(XKB_CONTEXT_NO_FLAGS));
}

Keyboard::~Keyboard()
{
    // needs version guarding, and the versioning here is messed up here, unless we're in sync with seat :/
    //        release();
}

void Keyboard::keyboard_keymap(uint32_t format, int32_t fd, uint32_t size)
{
    mKeymapFormat = format;
    if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) {
        close(fd);
        return;
    }

    char *map_str = static_cast<char *>(mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0));
    if (map_str == MAP_FAILED) {
        close(fd);
        return;
    }

    mXkbKeymap.reset(xkb_keymap_new_from_string(mXkbContext.get(), map_str, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS));
    QXkbCommon::verifyHasLatinLayout(mXkbKeymap.get());

    munmap(map_str, size);
    close(fd);

    if (mXkbKeymap)
        mXkbState.reset(xkb_state_new(mXkbKeymap.get()));
    else
        mXkbState.reset(nullptr);

    // KWin does not include compose options (e.g. compose:ralt) in the keymap it sends
    // to the input method's grabbed keyboard. Build a secondary state from kxkbrc that
    // does include those options so that the compose key produces Multi_key.
    buildComposeAwareKeymap();
}

void Keyboard::buildComposeAwareKeymap()
{
    mComposeAwareState.reset(nullptr);

    const KSharedConfig::Ptr config = KSharedConfig::openConfig(QStringLiteral("kxkbrc"), KConfig::NoGlobals);
    const KConfigGroup layoutGroup = config->group(QStringLiteral("Layout"));
    const QString options = layoutGroup.readEntry("Options", QString());

    if (options.isEmpty() || !options.contains(QLatin1String("compose:"))) {
        // No compose option configured; the compositor's keymap is sufficient.
        return;
    }

    // Use the first layout/variant from the list (the active one at startup).
    const QString layoutList = layoutGroup.readEntry("LayoutList", layoutGroup.readEntry("Layout", QStringLiteral("us")));
    const QString variantList = layoutGroup.readEntry("VariantList", layoutGroup.readEntry("Variant", QString()));
    const QByteArray model = layoutGroup.readEntry("Model", QStringLiteral("pc105")).toUtf8();
    const QByteArray layout = layoutList.section(QLatin1Char(','), 0, 0).trimmed().toUtf8();
    const QByteArray variant = variantList.section(QLatin1Char(','), 0, 0).trimmed().toUtf8();
    const QByteArray optionsBytes = options.toUtf8();

    const xkb_rule_names names = {
        .rules = "evdev",
        .model = model.constData(),
        .layout = layout.constData(),
        .variant = variant.constData(),
        .options = optionsBytes.constData(),
    };

    xkb_keymap *composeKeymap = xkb_keymap_new_from_names(mXkbContext.get(), &names, XKB_KEYMAP_COMPILE_NO_FLAGS);
    if (!composeKeymap) {
        qCWarning(PlasmaKeyboard) << "Failed to build compose-aware XKB keymap with options:" << options;
        return;
    }

    mComposeAwareState.reset(xkb_state_new(composeKeymap));
    xkb_keymap_unref(composeKeymap);

    if (!mComposeAwareState) {
        qCWarning(PlasmaKeyboard) << "Failed to create XKB state for compose-aware keymap";
    } else {
        qCDebug(PlasmaKeyboard) << "Compose-aware XKB keymap built with options:" << options;
    }
}

void Keyboard::keyboard_key(uint32_t serial, uint32_t time, uint32_t key, uint32_t state)
{
    auto code = key + 8; // map to wl_keyboard::keymap_format::keymap_format_xkb_v1

    xkb_keysym_t sym = xkb_state_key_get_one_sym(mXkbState.get(), code);

    // KWin strips compose options (e.g. compose:ralt) from the keymap it sends to
    // input methods, so the compose key (e.g. RALT) arrives as ISO_Level3_Shift
    // instead of Multi_key. Check the compose-aware keymap: if it maps this key to
    // Multi_key, use that. For all other keys (dead keys, regular keys) use the
    // keysym from mXkbState, which reflects the actual layout KWin sent us.
    xkb_keysym_t nativeKeysym = sym;
    if (mComposeAwareState) {
        const xkb_keysym_t composeSym = xkb_state_key_get_one_sym(mComposeAwareState.get(), code);
        if (composeSym == XKB_KEY_Multi_key) {
            nativeKeysym = XKB_KEY_Multi_key;
        }
    }

    auto modifiers = QXkbCommon::modifiers(mXkbState.get());
    int qtkey = QXkbCommon::keysymToQtKey(sym, modifiers, mXkbState.get(), code);
    QString text = QXkbCommon::lookupString(mXkbState.get(), code);

    // wl_keyboard::key_state: 0 = released, 1 = pressed, 2 = repeated (since v10).
    // Repeated is semantically a press with auto-repeat.
    const bool isRepeat = (state == 2);
    QEvent::Type type = (state == 0) ? QEvent::KeyRelease : QEvent::KeyPress;

    QKeyEvent keyEvent(type, qtkey, modifiers, key, nativeKeysym, 0, text, isRepeat);
    keyEvent.setAccepted(false);

    if (type == QEvent::KeyPress) {
        Q_EMIT keyPressed(&keyEvent);
    } else {
        Q_EMIT keyReleased(&keyEvent);
    }

    if (!keyEvent.isAccepted()) {
        m_parent->key(serial, time, key, state);
    }
}

void Keyboard::keyboard_modifiers(uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group)
{
    xkb_state_update_mask(mXkbState.get(), mods_depressed, mods_latched, mods_locked, 0, 0, group);
    if (mComposeAwareState) {
        xkb_state_update_mask(mComposeAwareState.get(), mods_depressed, mods_latched, mods_locked, 0, 0, group);
    }

    // currently not filterable
    m_parent->modifiers(serial, mods_depressed, mods_latched, mods_locked, group);
}

#include "moc_inputmethod_p.cpp"
