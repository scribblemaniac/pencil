#ifndef LAYERLABEL_H
#define LAYERLABEL_H

#include <QLabel>

class LayerLabel : public QLabel
{
    Q_OBJECT

public:
    LayerLabel(const QString& text) : QLabel(text) {}
    void mouseDoubleClickEvent( QMouseEvent * e ) { emit doubleClicked(); }

signals:
    void doubleClicked();
};

#endif // LAYERLABEL_H
