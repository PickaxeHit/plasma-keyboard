<!--
  - SPDX-FileCopyrightText: None
  - SPDX-License-Identifier: CC0-1.0
-->

# Plasma Keyboard

The plasma-keyboard is a virtual keyboard based on [Qt Virtual Keyboard](https://doc.qt.io/qt-6/qtvirtualkeyboard-overview.html) designed to integrate in Plasma.

It wraps Qt Virtual Keyboard in a window, and uses the input-method-v1 Wayland protocol to communicate with the compositor to function as an input method.

## Build and install

```sh
mkdir build && cd build
cmake ..
make && make install
```

## Install using the flatpak nightly repository

https://cdn.kde.org/flatpak/plasma-keyboard-nightly/org.kde.plasma.keyboard.flatpakref

See also: https://userbase.kde.org/Tutorials/Flatpak#Nightly_KDE_apps

## Layouts

The keyboard layouts are located in the [layouts](/layouts) folder.

## Input engines



## Troubleshooting

KWin by default only shows the keyboard when a text field is interacted with by touch. Set `KWIN_IM_SHOW_ALWAYS=1` when starting KWin (or the login session) in order to force the keyboard to always pop up.

