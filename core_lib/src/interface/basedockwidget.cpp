/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include <QMoveEvent>
#include <QGuiApplication>
#include <QScreen>
#include <QStyle>
#include <QDebug>

#include "basedockwidget.h"
#include "platformhandler.h"

BaseDockWidget::BaseDockWidget(QWidget* pParent) 
: QDockWidget(pParent, Qt::Tool)
{

#ifdef __APPLE__
    if (PlatformHandler::isDarkMode())
    {
        setStyleSheet("QDockWidget::title { background: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #4e4f4d, stop: 1 #424241);  "
                      "background-repeat: repeat-x; text-align: center;"
                      "border-style: solid; border-bottom-color: #000000;"
                      "border-width: 1px; }");
    }
#endif

}

BaseDockWidget::~BaseDockWidget()
{
}

void BaseDockWidget::moveEvent(QMoveEvent *event) {
    QDockWidget::moveEvent(event);
    if(!isFloating()) return;
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    int x = qMax(qMin(screenGeometry.right() - geometry().width(), event->pos().x()), screenGeometry.left());
    int y = qMax(qMin(screenGeometry.bottom() - geometry().height(), event->pos().y()), screenGeometry.top() + (titleBarWidget() != nullptr ? titleBarWidget()->height() : style()->pixelMetric(QStyle::PM_TitleBarHeight)));

    if (x != event->pos().x() || y != event->pos().y())
    {
        setGeometry(QRect(x, y, geometry().width(), geometry().height()));
    }
}
