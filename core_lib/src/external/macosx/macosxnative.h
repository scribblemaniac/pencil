#ifndef MACOSXNATIVE_H
#define MACOSXNATIVE_H

#include <QMainWindow>

namespace MacOSXNative
{
    void removeUnwantedMenuItems();

    void setAppearance(bool isDarkMode);
    bool isMouseCoalescingEnabled();
    void setWindowTitleBarAppearance(QMainWindow* window, const QColor& color);
    void setMouseCoalescingEnabled(bool enabled);
    bool isDarkMode();
}

#endif // MACOSXNATIVE_H
