#ifndef MACOSXNATIVE_H
#define MACOSXNATIVE_H

namespace MacOSXNative
{
    void removeUnwantedMenuItems();

    void setAppearance(bool isDarkMode);
    bool isMouseCoalescingEnabled();
    void setMouseCoalescingEnabled(bool enabled);
    bool isDarkMode();
}

#endif // MACOSXNATIVE_H
