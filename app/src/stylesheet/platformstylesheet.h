#ifndef PLATFORMSTYLESHEET_H
#define PLATFORMSTYLESHEET_H

#include <QPalette>

#include "stylesheet/macos/macosstylesheet.h"

namespace PlatformStylesheet
{
    QString customStylesheet(const QPalette& palette, const QString styleId)
    {
        if (styleId.contains("mac")) {
            return macOSStylesheet(palette);
        }

        return QString();
    }
}

#endif // PLATFORMSTYLESHEET_H
