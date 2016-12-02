#ifndef TRACKLIST_H
#define TRACKLIST_H

#include <QGraphicsView>

class Editor;

class TrackList : public QGraphicsView
{
    Q_OBJECT

public:
    TrackList( Editor* editor, QGraphicsScene *scene, QWidget *parent = Q_NULLPTR ) : QGraphicsView( scene, parent ), mEditor(editor) {}

    void mousePressEvent( QMouseEvent *event );

signals:
    void layerClicked( int i );

private:
    Editor* mEditor;
};

#endif // TRACKLIST_H
