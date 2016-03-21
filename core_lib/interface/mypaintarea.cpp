/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2014 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include <QGraphicsRectItem>
#include <QBrush>
#include <QPixmap>

#include "mphandler.h"
#include "mpbrush.h"
#include "mpsurface.h"
#include "mptile.h"
#include "mypaintarea.h"

MypaintArea::MypaintArea()
{
    MPHandler *mypaint = MPHandler::handler();

    connect(mypaint, SIGNAL(newTile(MPSurface*, MPTile*)), this, SLOT(onNewTile(MPSurface*, MPTile*)));
    connect(mypaint, SIGNAL(updateTile(MPSurface*, MPTile*)), this, SLOT(onUpdateTile(MPSurface*, MPTile*)));

    // Set scene
    m_scene.setSceneRect(this->rect());
    setScene(&m_scene);
    setAlignment((Qt::AlignLeft | Qt::AlignTop));
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

MypaintArea::~MypaintArea()
{

}

void MypaintArea::onNewTile(MPSurface *surface, MPTile *tile)
{
    Q_UNUSED(surface);
    m_scene.addItem(tile);
}

void MypaintArea::onUpdateTile(MPSurface *surface, MPTile *tile)
{
    Q_UNUSED(surface);
    tile->update();
}

void MypaintArea::tabletEvent(QTabletEvent *event)
{
    switch (event->type()) {
        case QEvent::TabletPress:
            if (event->pointerType() == QTabletEvent::Pen) {
                using_tablet = true;
                MPHandler::handler()->startStroke();
                event->accept();
            }
        break;
        case QEvent::TabletRelease:
            if (event->pointerType() == QTabletEvent::Pen) {
                // Finalize the stroke sequence.
                using_tablet = false;
                event->accept();
            }
        break;
        case QEvent::TabletMove:
            if (event->pointerType() == QTabletEvent::Pen) {
                QPointF pt(mapToScene(event->pos()));
                MPHandler::handler()->strokeTo(pt.x(), pt.y(), event->pressure(), event->xTilt(),  event->yTilt());
                // stroke_to(m_brush, (MyPaintSurface *)m_surface, pt.x(), pt.y(), event->pressure(), CTILT(event->xTilt()),  CTILT(event->yTilt()));
                event->accept();
            }
        break;
        default:
        break;
    }
}

void MypaintArea::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    MPHandler::handler()->startStroke();
}

void MypaintArea::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    if (!using_tablet) {
        QPointF pt = mapToScene(event->pos());
        MPHandler::handler()->strokeTo(pt.x(), pt.y());
    }
}

void MypaintArea::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    // Finalize the stroke sequence.
}
