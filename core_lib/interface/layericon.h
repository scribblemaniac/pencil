#ifndef LAYERICON_H
#define LAYERICON_H

#include <QGraphicsPixmapItem>

class Layer;
class Editor;

class LayerIcon : public QGraphicsPixmapItem
{
public:
    LayerIcon( Layer* layer, Editor* editor, QGraphicsItem *parent );

    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    Layer* mLayer;
    Editor* mEditor;
};

#endif // LAYERICON_H
