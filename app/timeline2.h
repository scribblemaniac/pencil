#ifndef TIMELINE2_H
#define TIMELINE2_H

#include <QDockWidget>
#include "basedockwidget.h"

namespace Ui {
class Timeline2;
}

class Layer;
class QGraphicsItem;

class Timeline2 : public BaseDockWidget
{
    Q_OBJECT

public:
    explicit Timeline2(Editor* editor, QWidget* parent = 0);
    ~Timeline2();

    void initUI() override;
    void updateUI() override;

private:
    void addLayerTrack( QGraphicsItem* empty, Layer* layer );

    Ui::Timeline2* ui;
    Editor* mEditor;

    int mLastSelected;
};

#endif // TIMELINE2_H
