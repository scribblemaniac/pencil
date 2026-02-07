#ifndef MACOSSTYLESHEET_H
#define MACOSSTYLESHEET_H

#include <QPalette>

namespace PlatformStylesheet
{
    QString macOSStylesheet(const QPalette& palette);

    QColor darken(const QColor &color, int amount);
    QColor lighten(const QColor &color, int amount);

    QString toolbarStylesheet();
}

#endif // MACOSSTYLESHEET_H
