#include "keyframeheader.h"

#include <QGraphicsView>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>

#include "timelinecells.h"
#include "playbackmanager.h"

KeyframeHeader::KeyframeHeader( Editor* editor ) : QGraphicsScene(), mEditor( editor )
{
    // Set up preferences
    mPrefs = editor->preference();
    frameLength = mPrefs->getInt(SETTING::TIMELINE_SIZE);
    fontSize = mPrefs->getInt(SETTING::LABEL_FONT_SIZE);
    frameSize = mPrefs->getInt(SETTING::FRAME_SIZE);
    drawFrameNumber = mPrefs->isOn(SETTING::DRAW_LABEL);
    shortScrub = mPrefs->isOn(SETTING::SHORT_SCRUB);
    connect( mPrefs, &PreferenceManager::optionChanged, this, &KeyframeHeader::loadSetting );

    // Intialize QGraphicsView
    QGraphicsView* view = new QGraphicsView( this );
    view->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    view->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    view->setAlignment( Qt::AlignTop | Qt::AlignLeft );
    view->setBackgroundBrush( QBrush("#dcdcdc") );
    view->setFrameStyle( QFrame::NoFrame );

    // Some helpful variables
    int fps = mEditor->fps();
    int curFrame = mEditor->currentFrame() - 1;
    PlaybackManager* playback = mEditor->playback();
    int rangeStart = playback->markInFrame();
    int rangeEnd = playback->markOutFrame();

    // Add ticks
    for ( int i = 0; i < frameLength; i++ )
    {
        if ( i % fps == 0 )
        {
            // Add primary line (frame number is evenly divisible by fps)
            mTicks.append( addLine( frameSize * i, 0, frameSize * i, PRIMARY_TICK_HEIGHT, PRIMARY_TICK_COLOR ) );
        }
        else if (fps % 2 == 0 && i % fps == fps / 2)
        {
            // Add major line (frame number is evenly divisible by fps / 2 but not fps)
            mTicks.append( addLine( frameSize * i, 0, frameSize * i, MAJOR_TICK_HEIGHT, MAJOR_TICK_COLOR ) );
        }
        else
        {
            // Add minor tick (all frames not covered above)
            mTicks.append( addLine( frameSize * i, 0, frameSize * i, MINOR_TICK_HEIGHT, MINOR_TICK_COLOR ) );
        }
        // Set Z-Index for tick to 10
        mTicks.last()->setZValue( 10 );
    }

    // Add rectangle to highlight the current frame
    mCurrentFrameHighlight = addRect( frameSize * curFrame, 0, frameSize, shortScrub ? SHORT_SCRUB_HEIGHT : HEIGHT, CURRENT_FRAME_BORDER, CURRENT_FRAME_BACKGROUND );
    // Set Z-Index for current frame highlight to 5
    mCurrentFrameHighlight->setZValue(5);

    // Add range highlight
    mRangeHighlight = addRect( frameSize * rangeStart, 0, frameSize * rangeEnd, 1, QPen(), RANGE_COLOR );
    // Only show range highlight if ranged playback is enabled
    mRangeHighlight->setVisible( playback->isRangedPlayback() );
    // Set Z-Index for current frame highlight to 7
    mRangeHighlight->setZValue( 7 );

    // Add text for first frame (special case)
    // Create text object
    QGraphicsTextItem* firstLabel = addText( "1", LABEL_FONT );
    mLabels.append( firstLabel );
    // Center the label in frame
    firstLabel->setPos( frameSize / 2.0 - firstLabel->boundingRect().width() / 2.0, LABEL_SPACING_TOP );
    // Set default label color (esp. for unformatted text)
    firstLabel->setDefaultTextColor( LABEL_COLOR );
    // Set Z-Index for label to 10
    firstLabel->setZValue( 10 );

    // Add text labels at fps intervals
    for( int i = fps; i < frameLength; i += fps )
    {
        // Create text object
        QGraphicsTextItem* label = addText( QString::number( i ), LABEL_FONT );
        mLabels.append( label );
        // Center the label in frame
        label->setPos( frameSize * ( i + 0.5 ) - label->boundingRect().width() / 2.0, LABEL_SPACING_TOP );
        // Set default label color (esp. for unformatted text)
        label->setDefaultTextColor( LABEL_COLOR );
        // Set Z-Index for label to 10
        firstLabel->setZValue( 10 );
    }
}

// Update preferences
// TODO properly handle updates
void KeyframeHeader::loadSetting(SETTING setting)
{
    switch (setting)
    {
    case SETTING::TIMELINE_SIZE:
        frameLength = mPrefs->getInt(SETTING::TIMELINE_SIZE);
        break;
    case SETTING::LABEL_FONT_SIZE:
        fontSize = mPrefs->getInt(SETTING::LABEL_FONT_SIZE);
        break;
    case SETTING::FRAME_SIZE:
        frameSize = mPrefs->getInt(SETTING::FRAME_SIZE);
        break;
    case SETTING::SHORT_SCRUB:
        shortScrub = mPrefs->isOn(SETTING::SHORT_SCRUB);
        break;
    case SETTING::DRAW_LABEL:
        drawFrameNumber = mPrefs->isOn(SETTING::DRAW_LABEL);
        break;
    default:
        break;
    }
}
