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

    void setWindowTitleBarAppearance(QMainWindow* window, const QColor& color)
    {
        MacOSXNative::setWindowTitleBarAppearance(window, color);
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

    QString customToolBarStylesheet(const QPalette& palette)
    {
        QString stylesheet = QString(R"(

            QToolBar {
                background-color: palette(window);
                border-bottom: 1px solid palette(placeholderText);
                spacing: 2px;
                padding: 2px;
            }

            QToolBar::separator {
                background-color: palette(placeholderText);
                width: 1px;
                margin: 4px 2px;
            }
        )");

        return stylesheet;
    }

    /// Certain macOS native components such as QToolBar do not inherit from QPalette, to fix that
    /// We define our own custom stylesheet.
    QString customStyleSheet(const QPalette& palette)
    {
        QString buttonColor = palette.color(QPalette::Button).name();

        // We determine based on the lightness of the window color whether we're in light or dark mode
        bool isDark = palette.color(QPalette::Window).lightness() < 128;

        QColor hover  = isDark ? lighten(buttonColor, 15) : darken(buttonColor, 15);
        QColor pressed = isDark ? lighten(buttonColor, 30) : darken(buttonColor, 30);

        QString toolbarStylesheet = customToolBarStylesheet(palette);

        return toolbarStylesheet + QString(R"(

            QToolButton {
                color: palette(button);
                background: transparent;
                border: 1px solid transparent;
                border-radius: 4px;
                padding: 4px;
            }

            QToolButton:hover {
                background-color: %1;
                border-color: palette(mid);
            }

            QToolBar QToolButton:pressed {
                background-color: %2;
            }

            QToolButton:pressed:hover {
                background-color: %2;
            }


            QToolButton:checked {
                background-color: palette(mid);
                color: palette(base);
            }

            QToolButton:disabled {
                color: palette(placeholderText);
                background-color: transparent;
            }

            QToolButton::menu-indicator {
                image: none;
            }

            TitleBarWidget QToolButton {
                color: palette(windowText);
                background: transparent;
                border: 0px;
                border-radius: 0px;
                padding: 0px;
            }

            TitleBarWidget QToolButton:hover {
                background-color: transparent;
                border: none;
            }

            TimeLine QToolButton {
                padding: 0;
            }

        )")
           .arg(hover.name())
           .arg(pressed.name());
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
