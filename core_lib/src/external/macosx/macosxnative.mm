#include "macosxnative.h"

#include <AppKit/NSWindow.h>
#include <AppKit/Appkit.h>
#include <Availability.h>

#include <QMainWindow>
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

            // HACK: For some reason certain UI elements do not update their appearances according the palette
            // QSpinBoxes is one of those examples.
            // Through trial and error I figured out that setting the window appearance
            // Fixes the problem.
            for (NSWindow *window in [NSApplication sharedApplication].windows)
            {
                window.appearance = appearance;
            }
        }
    }

    /// Sets the background color of the macOS window titlebar to the same as the input color
    void setWindowTitleBarAppearance(QMainWindow* window, const QColor& color)
    {
        NSView* view = (NSView *)window->winId();
        NSWindow* nsWindow = view.window;
        nsWindow.titlebarAppearsTransparent = YES;
        nsWindow.backgroundColor = [NSColor colorWithRed:color.redF()
                                                         green:color.greenF()
                                                         blue:color.blueF()
                                                         alpha:color.alphaF()];
    }
}
