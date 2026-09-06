/*
    SPDX-FileCopyrightText: 2013, 2016, 2017 Martin Gräßlin <mgraesslin@kde.org>
    SPDX-FileCopyrightText: 2026 Nikolay Kochulin <basiqueevangelist@yandex.ru>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "layoutswitch.h"
#include "inputmethod_p.h"
#include "inputplugin.h"
#include "keyboardlayoutsinterface.h"
#include <KGlobalAccel>
#include <KLocalizedString>
#include <QAction>
#include <xkbcommon/xkbcommon.h>

LayoutSwitch::LayoutSwitch(InputPlugin *inputPlugin, QObject *parent)
    : QObject(parent)
    , m_inputPlugin(inputPlugin)
{
    m_retypeAction = new QAction(i18nc("@action global shortcut", "Retype text in next keyboard layout"), this);
    m_retypeAction->setObjectName("retype_text_layout");

    KGlobalAccel::self()->setDefaultShortcut(m_retypeAction, {Qt::Key_Pause});
    KGlobalAccel::self()->setShortcut(m_retypeAction, {Qt::Key_Pause});

    connect(m_retypeAction, &QAction::triggered, this, &LayoutSwitch::manualRetype);
}

void LayoutSwitch::manualRetype()
{
    if (!m_inputPlugin->hasContext()) {
        return;
    }

    QByteArray surroundingText = m_inputPlugin->surroundingText().toUtf8();
    int start = qMin(m_inputPlugin->cursorPos(), m_inputPlugin->anchorPos());
    int end = qMax(m_inputPlugin->cursorPos(), m_inputPlugin->anchorPos());

    int deleteBefore = 0;

    if (start == end) {
        start = end = m_inputPlugin->cursorPos();

        while (start > 0 && !QChar::fromLatin1(surroundingText[start - 1]).isSpace()) {
            start--;
        }

        deleteBefore = end - start;
    }

    QString selection = QString::fromUtf8(surroundingText.mid(start, end - start));

    QString transformed;

    xkb_layout_index_t layouts = xkb_keymap_num_layouts(m_inputPlugin->keyboard()->xkb_keymap());
    xkb_layout_index_t currentLayout = xkb_state_serialize_layout(m_inputPlugin->keyboard()->xkb_state(), XKB_STATE_LAYOUT_EFFECTIVE);

    for (QChar c : std::as_const(selection)) {
        if (c.isSurrogate()) {
            // TODO: handle this properly.
            transformed.append(c);
            continue;
        }

        xkb_keysym_t keysym = xkb_utf32_to_keysym(c.unicode());

        xkb_keymap *keymap = m_inputPlugin->keyboard()->xkb_keymap();

        xkb_layout_index_t srcLayout = currentLayout;

        do {
            xkb_layout_index_t targetLayout = (srcLayout + 1) % layouts;

            const xkb_keycode_t max = xkb_keymap_max_keycode(keymap);
            for (xkb_keycode_t keycode = xkb_keymap_min_keycode(keymap); keycode < max; keycode++) {
                uint levelCount = xkb_keymap_num_levels_for_key(keymap, keycode, srcLayout);
                for (uint currentLevel = 0; currentLevel < levelCount; currentLevel++) {
                    const xkb_keysym_t *syms;
                    uint num_syms = xkb_keymap_key_get_syms_by_level(keymap, keycode, srcLayout, currentLevel, &syms);
                    for (uint sym = 0; sym < num_syms; sym++) {
                        if (syms[sym] == keysym) {
                            const xkb_keysym_t *outputSyms;
                            int symCount = xkb_keymap_key_get_syms_by_level(keymap, keycode, targetLayout, currentLevel, &outputSyms);

                            for (int i = 0; i < symCount; i++) {
                                uint32_t outputC = xkb_keysym_to_utf32(outputSyms[i]);

                                if (outputC == 0) {
                                    continue;
                                }

                                transformed.append(QChar::fromUcs4(outputC));
                            }

                            goto outer;
                        }
                    }
                }
            }

            srcLayout = targetLayout;
        } while (srcLayout != currentLayout);

        transformed.append(c);
    outer:;
    }

    OrgKdeKeyboardLayoutsInterface layoutsIface(QStringLiteral("org.kde.keyboard"), QStringLiteral("/Layouts"), QDBusConnection::sessionBus(), this);
    layoutsIface.switchToNextLayout();

    if (deleteBefore > 0) {
        m_inputPlugin->deleteSurroundingText(-deleteBefore, deleteBefore);
    }

    m_inputPlugin->commit(transformed);
}

#include "moc_layoutswitch.cpp"
