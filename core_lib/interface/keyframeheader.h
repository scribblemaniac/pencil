#ifndef KEYFRAMEHEADER_H
#define KEYFRAMEHEADER_H

#include <QGraphicsScene>
#include <QColor>

#include "preferencemanager.h"

class TimeLineCells;
class Editor;

class KeyframeHeader : public QGraphicsScene
{
public:
    KeyframeHeader( Editor* editor );

private:
    Editor* mEditor;

    // Preferences
    PreferenceManager* mPrefs;
    int frameLength, frameSize, fontSize;
    bool shortScrub, drawFrameNumber;

    // QGraphicsItems
    QList<QGraphicsLineItem*> mTicks;
    QGraphicsRectItem* mCurrentFrameHighlight;
    QGraphicsRectItem* mRangeHighlight;
    QList<QGraphicsTextItem*> mLabels;

    // Constant measurements
    const int HEIGHT = 24;
    const int PRIMARY_TICK_HEIGHT = 4;
    const int MAJOR_TICK_HEIGHT = 4;
    const int MINOR_TICK_HEIGHT = 2;
    const int LABEL_SPACING_TOP = 5;
    const int SHORT_SCRUB_HEIGHT = 22;

    // Theme relevant stuff
    const QColor PRIMARY_TICK_COLOR = { 70, 70, 70, 255 };
    const QColor MAJOR_TICK_COLOR = { 70, 70, 70, 255 };
    const QColor MINOR_TICK_COLOR = { 70, 70, 70, 255 };
    const QPen CURRENT_FRAME_BORDER = Qt::NoPen;
    const QBrush CURRENT_FRAME_BACKGROUND = { QColor( 255, 0, 0, 128 ) };
    const QBrush RANGE_COLOR = { Qt::yellow };
    const QFont LABEL_FONT = { "helvetica", 10 };
    const QColor LABEL_COLOR = { 70, 70, 70, 255 };

private slots:
    void loadSetting(SETTING setting);
};

#endif // KEYFRAMEHEADER_H
