#ifndef MACOSSTYLESHEET_H
#define MACOSSTYLESHEET_H

#include <QPalette>

class ThemeColorPalette;

namespace PlatformStylesheet
{
    QString macOSStylesheet(const ThemeColorPalette& palette);

    QString toolbarStylesheet();
}

#endif // MACOSSTYLESHEET_H
