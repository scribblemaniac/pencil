/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include <QApplication>
#include <QPalette>

#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
  #include <QOperatingSystemVersion>
#endif

#include <CoreFoundation/CoreFoundation.h>

#include "macosxnative.h"

namespace PlatformHandler
{
    void configurePlatformSpecificSettings()
    {
        MacOSXNative::removeUnwantedMenuItems();
    }

    bool isDarkMode()
    {
        return MacOSXNative::isDarkMode();
    }

    void setAppearanceIfPossible(bool isDarkMode)
    {
        MacOSXNative::setAppearance(isDarkMode);
    }

    QColor darken(const QColor &color, int amount)
    {
        int h, s, l, a;
        color.getHsl(&h, &s, &l, &a);
        l = qMax(l - amount, 0);
        return QColor::fromHsl(h, s, l, a);
    }

    QColor lighten(const QColor &color, int amount)
    {
        int h, s, l, a;
        color.getHsl(&h, &s, &l, &a);
        l = qMin(l + amount, 255);
        return QColor::fromHsl(h, s, l, a);
    }

    /// The QToolBar does natively not inherit from QPalette
    /// As such we need to create our own stylesheet which overrides the system styling.
    QString toolBarStyleSheet(const QPalette& palette)
    {
        QString windowColor     = palette.color(QPalette::Window).name();
        QString windowTextColor = palette.color(QPalette::WindowText).name();
        QString highlightColor  = palette.color(QPalette::Highlight).name();
        QString midColor        = palette.color(QPalette::Mid).name();
        QString disabledColor   = palette.color(QPalette::PlaceholderText).name();
        QString baseColor       = palette.color(QPalette::Base).name();

        // Darken/lighten for hover/pressed states relative to window color
        bool isDark = palette.color(QPalette::Window).lightness() < 128;

        QColor hover  = isDark ? lighten(windowColor, 15) : darken(windowColor, 15);
        QColor pressed = isDark ? lighten(windowColor, 30) : darken(windowColor, 30);

        return QString(R"(
            QToolBar {
                background-color: %1;
                border-bottom: 1px solid %5;
                spacing: 2px;
                padding: 2px;
            }

            QToolBar::separator {
                background-color: %5;
                width: 1px;
                margin: 4px 2px;
            }

            QToolBar QToolButton {
                color: %2;
                background: transparent;
                border: 1px solid transparent;
                border-radius: 4px;
                padding: 4px;
            }

            QToolBar QToolButton:hover {
                background-color: %6;
                border-color: %5;
            }

            QToolBar QToolButton:pressed {
                background-color: %7;
            }

            QToolBar QToolButton:checked {
                background-color: %3;
                color: %8;
            }

            QToolBar QToolButton:disabled {
                color: %4;
            }

            QToolBar QToolButton[popupMode="1"] ::menu-indicator {
                image: none;
                border: none;
            }
        )").arg(windowColor)        // %1 - toolbar background
          .arg(windowTextColor)     // %2 - button text
          .arg(highlightColor)      // %3 - checked/active state
          .arg(disabledColor)       // %4 - disabled text
          .arg(midColor)            // %5 - borders/separators
          .arg(hover.name())        // %6 - hover background
          .arg(pressed.name())      // %7 - pressed background
          .arg(baseColor);          // %8 - text on highlighted bg
    }

    void initialise()
    {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    }
}

extern "C" {

bool gIsMouseCoalescing = false;

void detectWhichOSX()
{
 #if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
    QOperatingSystemVersion current = QOperatingSystemVersion::current();
    gIsMouseCoalescing = ( current >= QOperatingSystemVersion::OSXElCapitan );
#else
    gIsMouseCoalescing = false;
#endif
}

void disableCoalescing()
{
    MacOSXNative::setMouseCoalescingEnabled(gIsMouseCoalescing);
}

void enableCoalescing()
{
    MacOSXNative::setMouseCoalescingEnabled(true);
}

} // extern "C"
