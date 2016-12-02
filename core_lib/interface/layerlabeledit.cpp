#include "layerlabeledit.h"

#include <QDebug>

LayerLabelEdit::LayerLabelEdit() :
    QLineEdit()
{
    connect(this, &LayerLabelEdit::editingFinished, [&] () { emit finished(); });
}

void LayerLabelEdit::setOriginalText(const QString &text)
{
    mOriginalText = text;
    setText(text);
}

void LayerLabelEdit::keyPressEvent(QKeyEvent *event)
{
    qDebug() << "Keypress" << event->key();
    if ( event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
    {
        emit finished();
    }
    else if ( event->key() == Qt::Key_Escape )
    {
        setText(mOriginalText);
        emit finished();
    }
    else
    {
        QLineEdit::keyPressEvent(event);
    }
}
