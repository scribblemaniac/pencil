/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include "titlebarwidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QIcon>
#include <QToolButton>
#include <QResizeEvent>
#include <QStyle>
#include <QPainter>
#include <QDebug>


#include "theming.h"
#include "platformhandler.h"
#include "buttonappearancewatcher.h"
#include "preferencemanager.h"

TitleBarWidget::TitleBarWidget(QWidget* parent)
    : QWidget(parent)
{

    QVBoxLayout* vLayout = new QVBoxLayout();

    vLayout->setContentsMargins(3,4,3,4);
    vLayout->setSpacing(0);

    vLayout->addWidget(createCustomTitleBarWidget(this));

    setLayout(vLayout);
}

TitleBarWidget::~TitleBarWidget()
{
}

QWidget* TitleBarWidget::createCustomTitleBarWidget(QWidget* parent)
{
    bool isDarkmode = PlatformHandler::isDarkMode();
    QWidget* containerWidget = new QWidget(parent);

    QHBoxLayout* containerLayout = new QHBoxLayout(parent);

    mCloseButton = new QToolButton(parent);

    mCloseButton->setStyleSheet(flatButtonStylesheet());

    QSize iconSize = QSize(14,14);
    QSize padding = QSize(2,2);

    mCloseButtonRes.lightMode = QIcon("://icons/themes/playful/window/window-close-button-normal.svg");
    mCloseButtonRes.darkMode = QIcon("://icons/themes/playful/window/window-close-button-normal-darkm.svg");

    QIcon closeIcon = mCloseButtonRes.iconForMode(isDarkmode);

    mCloseHoverButtonRes.lightMode = QIcon("://icons/themes/playful/window/window-close-button-active.svg");
    mCloseHoverButtonRes.darkMode = mCloseHoverButtonRes.lightMode;

    mCloseButton->setIcon(closeIcon);
    mCloseButton->setIconSize(iconSize);
    mCloseButton->setFixedSize(iconSize + padding);

    connect(mCloseButton, &QToolButton::clicked, this, &TitleBarWidget::closeButtonPressed);

    mDockButtonRes.lightMode = QIcon("://icons/themes/playful/window/window-float-button-normal.svg");
    mDockButtonRes.darkMode = QIcon("://icons/themes/playful/window/window-float-button-normal-darkm.svg");

    mDockHoverButtonRes.lightMode = QIcon("://icons/themes/playful/window/window-float-button-active.svg");
    mDockHoverButtonRes.darkMode = mDockHoverButtonRes.lightMode;

    mDockButton = new QToolButton(parent);

    QIcon dockIcon = mDockButtonRes.iconForMode(isDarkmode);
    mDockButton->setIcon(dockIcon);
    mDockButton->setStyleSheet(flatButtonStylesheet());

    mDockButton->setIconSize(iconSize);
    mDockButton->setFixedSize(iconSize + padding);

    connect(mDockButton, &QToolButton::clicked, this, &TitleBarWidget::undockButtonPressed);

    mTitleLabel = new QLabel(parent);
    mTitleLabel->setAlignment(Qt::AlignVCenter);

#ifdef __APPLE__
    containerLayout->addWidget(mCloseButton);
    containerLayout->addWidget(mDockButton);
    containerLayout->addWidget(mTitleLabel);
#else
    containerLayout->addWidget(mTitleLabel);
    containerLayout->addWidget(mDockButton);
    containerLayout->addWidget(mCloseButton);
#endif

    containerLayout->setSpacing(3);
    containerLayout->setContentsMargins(0,0,0,0);

    containerWidget->setLayout(containerLayout);
    containerWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    containerWidget->setMinimumSize(QSize(1,1));

    return containerWidget;
}

void TitleBarWidget::setupApperanceWatcher(PreferenceManager* preferenceManager)
{
    mDockButton->installEventFilter(new ButtonAppearanceWatcher(preferenceManager, mDockButtonRes, mDockHoverButtonRes, this));
    mCloseButton->installEventFilter(new ButtonAppearanceWatcher(preferenceManager, mCloseButtonRes, mCloseHoverButtonRes, this));
}

QString TitleBarWidget::flatButtonStylesheet() const
{
    return "QToolButton { border: 0; }";
}

void TitleBarWidget::setTitle(const QString &title)
{
    mTitleLabel->setText(title);
}

void TitleBarWidget::hideButtons(bool hide)
{
    mCloseButton->setHidden(hide);
    mDockButton->setHidden(hide);
}

void TitleBarWidget::resizeEvent(QResizeEvent *resizeEvent)
{
    QWidget::resizeEvent(resizeEvent);

    hideButtonsIfNeeded(resizeEvent->size().width());
}

void TitleBarWidget::hideButtonsIfNeeded(int width)
{
    if (width <= mWidthOfFullLayout) {
        hideButtons(true);
    } else {
        hideButtons(false);
    }
}

void TitleBarWidget::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    // This is to ensure that after the titlebar has been hidden with buttons hidden
    // the layout width is smaller, so we enable them again briefly to get the correct width.
    hideButtons(false);

    mWidthOfFullLayout = layout()->sizeHint().width();
    hideButtonsIfNeeded(size().width());
}

void TitleBarWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.save();
    painter.setBrush(palette().color(QPalette::Window));
    painter.setPen(Qt::NoPen);
    painter.drawRect(this->rect());
    painter.restore();
}
