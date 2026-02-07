#ifndef MACOSSTYLEFIXES_H
#define MACOSSTYLEFIXES_H

#include <QProxyStyle>
#include <QPainter>
#include <QStyleOption>

class MacOSPencilStyle : public QProxyStyle
{
public:
    explicit MacOSPencilStyle(QStyle* baseStyle = nullptr)
        : QProxyStyle(baseStyle) {
        setObjectName("macOSPencilStyle");
    }

    explicit MacOSPencilStyle(QString styleId)
        : QProxyStyle(styleId) {
        setObjectName("macOSPencilStyle");
    }

    void drawPrimitive(PrimitiveElement element,
                       const QStyleOption *option,
                       QPainter *painter,
                       const QWidget *widget) const override
    {
        if (widget && widget->inherits("QMainWindowTabBar")) {

            if (element == PE_FrameTabWidget) {
                // When dock widgets are tabified, they draw NSTabView like bezels but rather than clipping and putting
                // the content inside the area, the widgets are laid on top which looks wrong.
                // To prevent the bezels from being drawn, don't draw the frame here.
                return;
            };
        }

        QRectF rect = option->rect;
        if (element == PE_IndicatorToolBarHandle) {

            painter->setRenderHint(QPainter::Antialiasing);
            painter->setBrush(option->palette.color(QPalette::WindowText));
            painter->setPen(Qt::NoPen);

            qreal dotRadius = 1.0;
            qreal horizontalSpacing = 4.0;
            qreal verticalSpacing = 3.0;

            QPointF center = rect.center();

            /// Draws a two columns of dots
            if (rect.width() < rect.height()) {
                qreal leftX = center.x() - horizontalSpacing / 2;
                qreal rightX = center.x() + horizontalSpacing / 2;

                // Top
                painter->drawEllipse(QPointF(leftX, center.y() - verticalSpacing), dotRadius, dotRadius);
                painter->drawEllipse(QPointF(rightX, center.y() - verticalSpacing), dotRadius, dotRadius);

                // Middle
                painter->drawEllipse(QPointF(leftX, center.y()), dotRadius, dotRadius);
                painter->drawEllipse(QPointF(rightX, center.y()), dotRadius, dotRadius);

                // Bottom
                painter->drawEllipse(QPointF(leftX, center.y() + verticalSpacing), dotRadius, dotRadius);
                painter->drawEllipse(QPointF(rightX, center.y() + verticalSpacing), dotRadius, dotRadius);
            }

            return;
        } else if (element == PE_IndicatorToolBarSeparator) {

            // Draw Appkit lookalike separator, which conforms to the palette.
            painter->setRenderHint(QPainter::Antialiasing);
            painter->setPen(option->palette.color(QPalette::Mid));

            qreal verticalPadding = 6;

            painter->drawLine(QLine(rect.center().x(), verticalPadding, rect.center().x(), rect.bottom() - verticalPadding));

            return;
        }

        // Default behavior for everything else
        QProxyStyle::drawPrimitive(element, option, painter, widget);
    }
};

#endif // MACOSSTYLEFIXES_H
