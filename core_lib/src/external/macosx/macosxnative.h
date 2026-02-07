#ifndef MACOSXNATIVE_H
#define MACOSXNATIVE_H

#include <QMainWindow>

#include "pencildef.h"

namespace MacOSXNative
{
    void removeUnwantedMenuItems();

    void setAppearance(AppearanceMode mode);
    bool isMouseCoalescingEnabled();
    void setWindowTitleBarAppearance(QMainWindow* window, const QColor& color);
    void setMouseCoalescingEnabled(bool enabled);
    bool isDarkMode();
}

#endif // MACOSXNATIVE_H
