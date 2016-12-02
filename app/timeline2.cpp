#include "timeline2.h"
#include "ui_timeline2.h"

#include <QSplitter>
#include <QVBoxLayout>

#include "editor.h"

Timeline2::Timeline2(Editor *editor, QWidget *parent) : BaseDockWidget(parent)
{
    ui = new Ui::Timeline2;
    ui->setupUi(this);

    mEditor = editor;
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
    layerHeaderWrapper->setStyleSheet("background-color: lightgray;");
    layerHeaderWrapper->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    QHBoxLayout* layerHeaderWrapperLayout = new QHBoxLayout( layerHeaderWrapper );
    layerHeaderWrapperLayout->setAlignment(Qt::AlignLeft);

    // Toggle All Layer visibility button
    QPushButton* toggleAllVisibilityButton = new QPushButton( QIcon(":icons/eye_open.svg"), "");
    layerHeaderWrapperLayout->setSpacing(4);
    layerHeaderWrapperLayout->setMargin(2);
    toggleAllVisibilityButton->setStyleSheet("border: none;");
    toggleAllVisibilityButton->setIconSize( QSize( 16, 16 ) );
    toggleAllVisibilityButton->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum));
    layerHeaderWrapperLayout->addWidget(toggleAllVisibilityButton);
    connect(toggleAllVisibilityButton, &QPushButton::clicked, mEditor, &Editor::toggleShowAllLayers);

    // Temporary placeholder
    QLabel* test1 = new QLabel( "Keyframe", keyframeWrapper );

    splitter->setSizes( QList<int>() << 100 << 600 );
}

void Timeline2::updateUI()
{

}
