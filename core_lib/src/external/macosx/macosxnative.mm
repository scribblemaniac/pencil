#include "macosxnative.h"

#include <AppKit/NSWindow.h>
#include <AppKit/Appkit.h>
#include <Availability.h>

namespace MacOSXNative
{
    #if !defined(MAC_OS_X_VERSION_10_14) || MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_14
        NSString* const NSAppearanceNameDarkAqua = @"NSAppearanceNameDarkAqua";
    #endif

    void removeUnwantedMenuItems()
    {
        // Remove "Show Tab Bar" option from the "View" menu if possible

        if ([NSWindow respondsToSelector:@selector(allowsAutomaticWindowTabbing)])
        {
            NSWindow.allowsAutomaticWindowTabbing = NO;
        }
    }

    bool isMouseCoalescingEnabled()
    {
        return NSEvent.isMouseCoalescingEnabled;
    }

    void setMouseCoalescingEnabled(bool enabled)
    {
        NSEvent.mouseCoalescingEnabled = enabled;
    }

    bool isDarkMode()
    {
        if (@available(macOS 10.14, *))
        {
            NSAppearanceName appearance =
                [[NSApp effectiveAppearance] bestMatchFromAppearancesWithNames:@[
                  NSAppearanceNameAqua, NSAppearanceNameDarkAqua
                ]];
            return [appearance isEqual:NSAppearanceNameDarkAqua];
        }
        return false;
    }

    void setAppearance(bool isDarkMode)
    {
        if (@available(macOS 10.14, *))
        {
            NSAppearance *appearance = [NSAppearance appearanceNamed: isDarkMode ? NSAppearanceNameDarkAqua : NSAppearanceNameAqua];
            [NSApp setAppearance: appearance];

            // HACK: For some reason QSpinBoxes does not update set their appearance correctly..
            // Through trial and error I figured out that setting the window appearance
            // Fixes the problem.
            for (NSWindow *window in [NSApplication sharedApplication].windows)
            {
                window.appearance = appearance;
            }
        }
    }
}
