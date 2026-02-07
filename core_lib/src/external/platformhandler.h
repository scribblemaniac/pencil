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
#ifndef PLATFORMHANDLER_H
#define PLATFORMHANDLER_H

#include <QString>
#include <QPalette>
#include <QMainWindow>

namespace PlatformHandler
{

void configurePlatformSpecificSettings();
bool isDarkMode();

/* Forces the OS to use a certain appearance. This is particularly handy when
 * you change from light to dark palettes.
*/
void setAppearanceIfPossible(bool darkmode);

/* Changes the appearance of the respective OS title bar
 * Currently only used on macOS to change the background color of the title bar.
 */
void setWindowTitleBarAppearance(QMainWindow* window, const QColor& color);
void initialise();

}

#endif // PLATFORMHANDLER_H
