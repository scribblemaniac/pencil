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
#include "layericon.h"
#include "keyframeheader.h"

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
    QGraphicsRectItem* selectedBackground;
    for ( int i = 0; i < mEditor->object()->getLayerCount(); i++ )
    {
        QGraphicsItem* empty = new QGraphicsProxyWidget();
        empty->setPos( 0, i * 21 );
        layerTracks->addItem(empty);

        addLayerTrack( empty, mEditor->object()->getLayer( i ) );
    }
    // Final separator line
    layerTracks->addRect( 0, 21 * mEditor->object()->getLayerCount(), 1200, 1, Qt::NoPen, QBrush( Qt::black ) );

    // Selected layer background
    QLinearGradient linearGradient( QPointF( 0, 1 ), QPointF( 0, 21 ) );
    linearGradient.setColorAt( 0, QColor( 255, 255, 255, 128 ) );
    linearGradient.setColorAt( 0.50, QColor( 255, 255, 255, 64 ) );
    linearGradient.setColorAt( 1, QColor( 255, 255, 255, 0 ) );
    selectedBackground = layerTracks->addRect( 0, 1, 1200, 20, Qt::NoPen, linearGradient );
    selectedBackground->setPos( 0, 21 * mEditor->layers()->currentLayerIndex() );
    selectedBackground->setZValue( -1 );
    connect( mEditor->layers(), &LayerManager::currentLayerChanged, [&, layerTracksView, selectedBackground] ( int i ) {
        selectedBackground->setPos( 0, 21 * i + 1 );
        mLastSelected = i;
    } );

    // Create a wrapper for the keyframe header and tracks
    QWidget* keyframeWrapper = new QWidget( splitter );
    QVBoxLayout* keyframeWrapperLayout = new QVBoxLayout( keyframeWrapper );
    keyframeWrapperLayout->setMargin(0);
    keyframeWrapperLayout->setSpacing(0);

    // Keyframe header
    KeyframeHeader* keyframeHeader = new KeyframeHeader( mEditor );
    keyframeWrapperLayout->addWidget( keyframeHeader->views().first() );

    splitter->setSizes( QList<int>() << 100 << 600 );
}

void Timeline2::updateUI()
{

}

void Timeline2::addLayerTrack( QGraphicsItem* empty, Layer *layer )
{
    // Separator line
    QGraphicsRectItem* line = new QGraphicsRectItem( 0, 0, 1200, 1);
    line->setPen( Qt::NoPen );
    line->setBrush( Qt::black );
    line->setParentItem( empty );

    // Toggle visibility button
    QPushButton* toggleVisibilityButton;
    if ( layer->visible() )
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
    QGraphicsProxyWidget* toggleVisibilityButtonProxy = new QGraphicsProxyWidget();
    toggleVisibilityButtonProxy->setWidget( toggleVisibilityButton );
    toggleVisibilityButtonProxy->setParentItem( empty );
    toggleVisibilityButtonProxy->setPos( 4, 3 );
    connect(toggleVisibilityButton, &QPushButton::clicked, [&, layer, toggleVisibilityButton] () {
        for( int j = 0; j < mEditor->object()->getLayerCount(); j++ )
        {
            if ( mEditor->object()->getLayer( j ) == layer )
            {
                mEditor->switchVisibilityOfLayer( j );
                break;
            }
        }

        if ( layer->visible() )
        {
            toggleVisibilityButton->setIcon( QIcon( ":icons/eye_open.svg" ) );
        }
        else
        {
            toggleVisibilityButton->setIcon( QIcon( ":icons/eye_closed.svg" ) );
        }
    });

    // Layer type icon
    LayerIcon* layerIcon = new LayerIcon( layer, mEditor, empty );
    layerIcon->setPos( 24, 3 );

    // Label
    LayerLabel* labelWidget = new LayerLabel( layer->name() );
    labelWidget->setFont( QFont( "Helvetica", 12 ) );
    labelWidget->setStyleSheet( "background: transparent;" );
    connect(labelWidget, &LayerLabel::doubleClicked, labelWidget, &LayerLabel::hide);
    QGraphicsProxyWidget* labelProxy = new QGraphicsProxyWidget();
    labelProxy->setWidget( labelWidget );
    labelProxy->setParentItem( empty );
    labelProxy->setPos( 50, 6 );

    // Label editor
    LayerLabelEdit* labelEditWidget = new LayerLabelEdit();
    labelEditWidget->setOriginalText( layer->name() );
    labelEditWidget->hide();
    labelEditWidget->setFont( QFont( "Helvetica", 12 ) );
    labelEditWidget->setStyleSheet( "border: none; background: transparent; margin-left: -2px;" );
    labelEditWidget->setTextMargins( 0, 0, 0, 0 );
    connect(labelWidget, &LayerLabel::doubleClicked, [=] () { labelEditWidget->setOriginalText( labelWidget->text() ); });
    connect(labelWidget, &LayerLabel::doubleClicked, labelEditWidget, &LayerLabelEdit::show);
    connect(labelWidget, &LayerLabel::doubleClicked, [=] () { labelEditWidget->setFocus(); });
    connect(labelEditWidget, &LayerLabelEdit::finished, labelEditWidget, &QLineEdit::hide);
    connect(labelEditWidget, &LayerLabelEdit::finished, [=] () { labelWidget->setText( labelEditWidget->text() ); });
    connect(labelEditWidget, &LayerLabelEdit::finished, labelWidget, &QLineEdit::show);
    QGraphicsProxyWidget* labelEditProxy = new QGraphicsProxyWidget();
    labelEditProxy->setWidget( labelEditWidget );
    labelEditProxy->setParentItem( empty );
    labelEditProxy->setPos( 50, 4 );
}
