#ifndef LAYERLABELEDIT_H
#define LAYERLABELEDIT_H

#include <QLineEdit>
#include <QKeyEvent>

class LayerLabelEdit : public QLineEdit
{
    Q_OBJECT

public:
    LayerLabelEdit();

    void setOriginalText( const QString& text );
    void keyPressEvent(QKeyEvent *event);

signals:
    void finished();

private:
    QString mOriginalText;
};

#endif // LAYERLABELEDIT_H
