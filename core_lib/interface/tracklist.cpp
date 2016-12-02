#include <QMouseEvent>
#include <QGraphicsItem>

#include "editor.h"
#include "object.h"

#include "tracklist.h"

void TrackList::mousePressEvent(QMouseEvent *event)
{
    event->ignore();
    QGraphicsView::mousePressEvent(event);
    if ( event->isAccepted() ) return;

    int layerIndex = qFloor( mapToScene( event->pos() ).y() / 21 );
    if ( layerIndex >= 0 && layerIndex < mEditor->object()->getLayerCount() )
    {
        emit layerClicked( layerIndex );
        event->accept();
    }
}
