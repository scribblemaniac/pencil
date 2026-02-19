
#include <QFile>
#include <QUrl>

#include "macosstylesheet.h"
#include "util.h"

namespace PlatformStylesheet {

    /// Certain macOS native components such as QToolBar do not inherit from QPalette, to fix that
    /// We define our own custom stylesheet.
    QString macOSStylesheet(const QPalette& palette)
    {

        QString buttonColor     = palette.color(QPalette::Button).name();

        // We determine based on the lightness of the window color whether we're in light or dark mode
        bool isDark = palette.color(QPalette::Window).lightness() < 128;

        QColor hover  = isDark ? lighten(buttonColor, 15) : darken(buttonColor, 15);
        QColor pressed = isDark ? lighten(buttonColor, 30) : darken(buttonColor, 30);

        return toolbarStylesheet() + QString(R"(

            QMainWindow::separator {
                /* Fixes separator color being incorrect */
                border: 1px solid palette(mid);
            }

            /*
                The closest thing to a native UI component on macOS is NSButton with imageStyle
                However nothing looks quite right, as such we're creating our own button style instead
            */
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

            /*
                Small tweaks to the title bar like removing the padding again, removing background and setting no border
                The the buttons looks more native.
            */
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

    /// The toolbar does not out of the box style according to the palette, as such
    /// we enforce our own style to make sure it shares the same look and feel
    QString toolbarStylesheet()
    {

        QString stylesheet = QString(R"(
            QToolBar {
                background-color: palette(window);
                border-bottom: 1px solid palette(mid);
                spacing: 2px;
                padding: 2px;
            }

            QToolBar::separator {
                background-color: palette(mid);
                width: 1px;
                margin: 4px 2px;
            }

        )");

        return stylesheet;
    }

}
