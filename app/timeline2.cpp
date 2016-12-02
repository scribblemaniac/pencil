#include "timeline2.h"
#include "ui_timeline2.h"

#include <QSplitter>
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QGraphicsProxyWidget>
#include <QLineEdit>

#include "editor.h"
#include "object.h"
#include "layerlabel.h"
#include "layerlabeledit.h"
#include "layermanager.h"
#include "tracklist.h"

Timeline2::Timeline2(Editor *editor, QWidget *parent) : BaseDockWidget(parent)
{
    ui = new Ui::Timeline2;
    ui->setupUi(this);

    mEditor = editor;
    mLastSelected = editor->layers()->currentLayerIndex();
}

Timeline2::~Timeline2()
{
    delete ui;
}

void Timeline2::initUI()
{
    // Hide timeline placeholder (it only exists for visual effect in QT Designer)
    ui->timelinePlaceholder->setVisible(false);

    // Create a splitter with the layerWrapper on the left and keyframeWrapper on the right
    QSplitter* splitter = new QSplitter( ui->dockWidgetContents );
    ui->dockWidgetContents->layout()->addWidget( splitter );

    // Create a wrapper for the layer header and tracks
    QWidget* layerWrapper = new QWidget( splitter );
    layerWrapper->setMinimumWidth(120);
    QVBoxLayout* layerWrapperLayout = new QVBoxLayout( layerWrapper );
    layerWrapperLayout->setMargin(0);
    layerWrapperLayout->setSpacing(0);

    // Create a wrapper for the keyframe header and tracks
    QWidget* keyframeWrapper = new QWidget( splitter );
    QVBoxLayout* keyframeWrapperLayout = new QVBoxLayout( keyframeWrapper );

    // Layer header
    QWidget* layerHeaderWrapper = new QWidget();
    layerWrapperLayout->addWidget(layerHeaderWrapper);
    layerHeaderWrapper->setStyleSheet("background-color: #dcdcdc;");
    layerHeaderWrapper->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    QHBoxLayout* layerHeaderWrapperLayout = new QHBoxLayout( layerHeaderWrapper );
    layerHeaderWrapperLayout->setAlignment(Qt::AlignLeft);
    layerHeaderWrapperLayout->setSpacing(4);
    layerHeaderWrapperLayout->setMargin(4);

    // Toggle All Layer visibility button
    QPushButton* toggleAllVisibilityButton = new QPushButton( QIcon(":icons/eye_open.svg"), "" );
    // Remove button styling
    toggleAllVisibilityButton->setStyleSheet( "background: transparent; border: none; padding: 0 -2px;" );
    toggleAllVisibilityButton->setIconSize( QSize( 16, 16 ) );
    toggleAllVisibilityButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
    layerHeaderWrapperLayout->addWidget( toggleAllVisibilityButton );
    connect( toggleAllVisibilityButton, &QPushButton::clicked, mEditor, &Editor::toggleShowAllLayers );

    // Layer body
    QGraphicsScene* layerTracks = new QGraphicsScene;
    TrackList* layerTracksView = new TrackList( mEditor, layerTracks );
    layerTracksView->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    layerTracksView->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    layerTracksView->setAlignment( Qt::AlignTop | Qt::AlignLeft );
    layerTracksView->setBackgroundBrush(QBrush("#c0c0c0"));
    layerTracksView->setFrameStyle(QFrame::NoFrame);
    connect(layerTracksView, &TrackList::layerClicked, [=] ( int i ) { mEditor->layers()->setCurrentLayer(i); } );
    layerWrapperLayout->addWidget( layerTracksView );

    // Layer tracks
    Layer* currentLayer;
    QGraphicsRectItem* selectedBackground;
    for ( int i = 0; i < mEditor->object()->getLayerCount(); i++ )
    {
        currentLayer = mEditor->object()->getLayer( i );

        // Selected layer background
        if(mEditor->layers()->currentLayer() == currentLayer) {
            QLinearGradient linearGradient( QPointF( 0, 1 ), QPointF( 0, 21 ) );
            linearGradient.setColorAt( 0, QColor( 255, 255, 255, 128 ) );
            linearGradient.setColorAt( 0.50, QColor( 255, 255, 255, 64 ) );
            linearGradient.setColorAt( 1, QColor( 255, 255, 255, 0 ) );
            selectedBackground = layerTracks->addRect( 0, 0, 1200, 20, Qt::NoPen, linearGradient );
            selectedBackground->setPos( 0, 21 * i + 1 );
            selectedBackground->setZValue(-1);
        }

        // Toggle visibility button
        QPushButton* toggleVisibilityButton;
        if ( currentLayer->visible() )
        {
            toggleVisibilityButton = new QPushButton( QIcon( ":icons/eye_open.svg" ), "" );
        }
        else
        {
            toggleVisibilityButton = new QPushButton( QIcon( ":icons/eye_closed.svg" ), "" );
        }
        toggleVisibilityButton->setStyleSheet( "background: transparent; border: none; padding: 0 -2px;" );
        toggleVisibilityButton->setIconSize( QSize( 16, 16 ) );
        toggleVisibilityButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
        QGraphicsProxyWidget* toggleVisibilityButtonProxy = layerTracks->addWidget( toggleVisibilityButton );
        toggleVisibilityButtonProxy->setPos( 4, 21 * i + 3 );
        connect(toggleVisibilityButton, &QPushButton::clicked, [&, currentLayer, toggleVisibilityButton] () {
            for( int j = 0; j < mEditor->object()->getLayerCount(); j++ ) {
                if ( mEditor->object()->getLayer( j )->id() == currentLayer->id() )
                {
                    mEditor->switchVisibilityOfLayer( j );
                    break;
                }
            }

            if ( currentLayer->visible() )
            {
                toggleVisibilityButton->setIcon( QIcon( ":icons/eye_open.svg" ) );
            }
            else
            {
                toggleVisibilityButton->setIcon( QIcon( ":icons/eye_closed.svg" ) );
            }
        });

        // Layer type icon
        QPixmap* typePixmap = Q_NULLPTR;
        switch( currentLayer->type() )
        {
        case Layer::BITMAP:
            typePixmap = new QPixmap( ":icons/layer-bitmap.png" );
            break;
        case Layer::VECTOR:
            typePixmap = new QPixmap( ":icons/layer-vector.png" );
            break;
        case Layer::SOUND:
            typePixmap = new QPixmap( ":icons/layer-sound.png" );
            break;
        case Layer::CAMERA:
            typePixmap = new QPixmap( ":icons/layer-camera.png" );
            break;
        }
        if( typePixmap )
        {
            QGraphicsPixmapItem* icon = layerTracks->addPixmap( *typePixmap );
            icon->setPos( 24, 21 * i + 3 );
        }

        // Label
        LayerLabel* labelWidget = new LayerLabel( currentLayer->name() );
        labelWidget->setFont( QFont( "Helvetica", 12 ) );
        labelWidget->setStyleSheet( "background: transparent;" );
        connect(labelWidget, &LayerLabel::doubleClicked, labelWidget, &LayerLabel::hide);
        QGraphicsProxyWidget* labelProxy = layerTracks->addWidget( labelWidget );
        labelProxy->setPos( 50, 21 * i + 6 );

        // Label editor
        LayerLabelEdit* labelEditWidget = new LayerLabelEdit( );
        labelEditWidget->setOriginalText( currentLayer->name() );
        labelEditWidget->hide();
        labelEditWidget->setFont( QFont( "Helvetica", 12 ) );
        labelEditWidget->setStyleSheet( "border: none; background: transparent; margin-left: -2px;");
        labelEditWidget->setTextMargins( 0, 0, 0, 0 );
        connect(labelWidget, &LayerLabel::doubleClicked, [=] () { labelEditWidget->setOriginalText( labelWidget->text() ); });
        connect(labelWidget, &LayerLabel::doubleClicked, labelEditWidget, &LayerLabelEdit::show);
        connect(labelWidget, &LayerLabel::doubleClicked, [=] () { labelEditWidget->setFocus(); });
        connect(labelEditWidget, &LayerLabelEdit::finished, labelEditWidget, &QLineEdit::hide);
        connect(labelEditWidget, &LayerLabelEdit::finished, [=] () { labelWidget->setText( labelEditWidget->text() ); });
        connect(labelEditWidget, &LayerLabelEdit::finished, labelWidget, &QLineEdit::show);
        QGraphicsProxyWidget* labelEditProxy = layerTracks->addWidget( labelEditWidget );
        labelEditProxy->setPos( 50, 21 * i + 4 );

        // Separator line
        layerTracks->addRect( 0, 21 * i, 1200, 1, Qt::NoPen, QBrush( Qt::black ) );
    }
    // Final separator line
    layerTracks->addRect( 0, 21 * mEditor->object()->getLayerCount(), 1200, 1, Qt::NoPen, QBrush( Qt::black ) );

    // Layer switching
    connect(mEditor->layers(), &LayerManager::currentLayerChanged, [&, layerTracksView, selectedBackground] ( int i ) {
        selectedBackground->setPos( 0, 21 * i + 1 );
        //layerTracksView->update( 0, mLastSelected * 21, 1200, mLastSelected * 21 + 21 );
        //layerTracksView->update( 0, i * 21, 1200, i * 21 + 21 );
        mLastSelected = i;
    } );

    // Temporary placeholder
    QLabel* test1 = new QLabel( "Keyframe", keyframeWrapper );

    splitter->setSizes( QList<int>() << 100 << 600 );
}

void Timeline2::updateUI()
{

}
