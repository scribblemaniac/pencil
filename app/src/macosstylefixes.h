#ifndef MACOSSTYLEFIXES_H
#define MACOSSTYLEFIXES_H

#include <QProxyStyle>

class MacOSStyleFixes : public QProxyStyle
{
public:
    using QProxyStyle::QProxyStyle;

    void drawPrimitive(PrimitiveElement element,
                       const QStyleOption *option,
                       QPainter *painter,
                       const QWidget *widget) const override
    {
        if (widget->inherits("QMainWindowTabBar")) {

            if (element == PE_FrameTabWidget) {
                // When dock widgets are tabified, they draw NSTabView like bezels but rather than clipping and putting
                // the content inside the area, the widgets are laid on top which looks wrong.
                // To prevent the bezels from being drawn, don't draw the frame here.
                return;
            }
        }

        QProxyStyle::drawPrimitive(element, option, painter, widget);
    }
};

#endif // MACOSSTYLEFIXES_H
