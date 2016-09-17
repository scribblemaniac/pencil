#include "timeline2.h"
#include "ui_timeline2.h"

#include <QSplitter>
#include <QVBoxLayout>

Timeline2::Timeline2(QWidget *parent) : BaseDockWidget(parent)
{
    ui = new Ui::Timeline2;
    ui->setupUi(this);
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
    splitter->setSizes( QList<int>() << 100 << 600 );
    ui->dockWidgetContents->layout()->addWidget( splitter );

    // Create a wrapper for the layer header and tracks
    QWidget* layerWrapper = new QWidget( splitter );
    QVBoxLayout* layerWrapperLayout = new QVBoxLayout( layerWrapper );

    // Create a wrapper for the keyframe header and tracks
    QWidget* keyframeWrapper = new QWidget( splitter );
    QVBoxLayout* keyframeWrapperLayout = new QVBoxLayout( keyframeWrapper );

    // Temporary placeholders
    QLabel* test = new QLabel( "Layers", layerWrapper );
    QLabel* test1 = new QLabel( "Keyframe", keyframeWrapper );
}

void Timeline2::updateUI()
{

}
