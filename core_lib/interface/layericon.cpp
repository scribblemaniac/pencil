#include "layericon.h"

#include <QGraphicsSceneMouseEvent>
#include "layer.h"
#include "editor.h"
#include "object.h"
#include "layermanager.h"

LayerIcon::LayerIcon(Layer* layer, Editor *editor, QGraphicsItem *parent = Q_NULLPTR ) : QGraphicsPixmapItem( parent ), mLayer(layer), mEditor(editor)
{
    switch( layer->type() )
    {
    case Layer::BITMAP:
        this->setPixmap(QPixmap( ":icons/layer-bitmap.png" ));
        break;
    case Layer::VECTOR:
        this->setPixmap(QPixmap( ":icons/layer-vector.png" ));
        break;
    case Layer::SOUND:
        this->setPixmap(QPixmap( ":icons/layer-sound.png" ));
        break;
    case Layer::CAMERA:
        this->setPixmap(QPixmap( ":icons/layer-camera.png" ));
        break;
    default:
        break;
    }
}

void LayerIcon::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    for( int i = 0; i < mEditor->object()->getLayerCount(); i++ )
    {
        if ( mEditor->object()->getLayer( i ) == mLayer )
        {
            mEditor->layers()->setCurrentLayer(i);
            return;
        }
    }
}
