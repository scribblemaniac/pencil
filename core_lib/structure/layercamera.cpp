/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2017 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include "layercamera.h"

#include <QLineEdit>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QtDebug>
#include "camera.h"



CameraPropertiesDialog::CameraPropertiesDialog(QString name, int width, int height) : QDialog()
{
    QLabel* nameLabel = new QLabel(tr("Camera name:"));
    nameBox = new QLineEdit();
    nameBox->setText(name);
    QHBoxLayout* nameLayout = new QHBoxLayout();
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameBox);

    QLabel* sizeLabel = new QLabel(tr("Camera size:"));
    widthBox = new QSpinBox();
    heightBox = new QSpinBox();
    widthBox->setValue(width);
    heightBox->setValue(height);
    widthBox->setMaximum(10000);
    heightBox->setMaximum(10000);
    widthBox->setMinimum(1);
    heightBox->setMinimum(1);
    QHBoxLayout* sizeLayout = new QHBoxLayout();
    sizeLayout->addWidget(sizeLabel);
    sizeLayout->addWidget(widthBox);
    sizeLayout->addWidget(heightBox);

    QPushButton* okButton = new QPushButton(tr("Ok"));
    QPushButton* cancelButton = new QPushButton(tr("Cancel"));
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    QGridLayout* layout = new QGridLayout();
    layout->addLayout(nameLayout, 0, 0);
    layout->addLayout(sizeLayout, 1, 0);
    layout->addLayout(buttonLayout, 2, 0);
    setLayout(layout);
    connect(okButton, SIGNAL(pressed()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(pressed()), this, SLOT(reject()));
}

CameraPropertiesDialog::~CameraPropertiesDialog()
{
}

QString CameraPropertiesDialog::getName()
{
    return nameBox->text();
}

void CameraPropertiesDialog::setName(QString name)
{
    nameBox->setText(name);
}

int CameraPropertiesDialog::getWidth()
{
    return widthBox->value();
}

void CameraPropertiesDialog::setWidth(int width)
{
    widthBox->setValue(width);
}

int CameraPropertiesDialog::getHeight()
{
    return heightBox->value();
}

void CameraPropertiesDialog::setHeight(int height)
{
    heightBox->setValue(height);
}

// ------

LayerCamera::LayerCamera( Object* object ) : Layer( object, Layer::CAMERA )
{
    setName(tr("Camera Layer"));
    viewRect = QRect(QPoint(-400, -300), QSize(800, 600));
    dialog = NULL;
}

LayerCamera::~LayerCamera()
{
}

Camera* LayerCamera::getCameraAtFrame(int frameNumber)
{
    return static_cast< Camera* >( getKeyFrameAt( frameNumber ) );
}

Camera* LayerCamera::getLastCameraAtFrame(int frameNumber, int increment)
{
    return static_cast< Camera* >( getLastKeyFrameAtPosition( frameNumber + increment ) );
}

QTransform LayerCamera::getViewAtFrame(int frameNumber)
{
    if ( keyFrameCount() == 0 )
    {
        return QTransform();
    }

    Camera* camera1 = static_cast< Camera* >( getLastKeyFrameAtPosition( frameNumber ) );

    int nextFrame = getNextKeyFramePosition( frameNumber );
	Camera* camera2 = static_cast< Camera* >( getLastKeyFrameAtPosition( nextFrame ) );

    if (camera1 == NULL && camera2 == NULL)
    {
        return QTransform();
    }
    else if (camera1 == NULL && camera2 != NULL)
    {
        return camera2->view;
    }
    else if (camera2 == NULL && camera1 != NULL)
    {
        return camera1->view;
    }

	if ( camera1 == camera2 )
	{
		return camera1->view;
	}

    double frame1 = camera1->pos();
    double frame2 = camera2->pos();
    
    // linear interpolation
    qreal c2 = ( frameNumber - frame1) / ( frame2 - frame1 );
    qreal c1 = 1.0 - c2;

    auto interpolation = [=]( double f1, double f2 ) -> double
    {
        return f1 * c1 + f2 * c2;
    };

    return QTransform( interpolation( camera1->view.m11(), camera2->view.m11() ),
                       interpolation( camera1->view.m12(), camera2->view.m12() ),
                       interpolation( camera1->view.m21(), camera2->view.m21() ),
                       interpolation( camera1->view.m22(), camera2->view.m22() ),
                       interpolation( camera1->view.dx(),  camera2->view.dx() ),
                       interpolation( camera1->view.dy(),  camera2->view.dy() ) );
   
}

void LayerCamera::LinearInterpolateTransform(Camera* cam)
{
    Q_ASSERT(keyFrameCount() > 0);
    
    int frameNumber = cam->pos();
    Camera* camera1 = static_cast<Camera*>(getLastKeyFrameAtPosition(frameNumber - 1));

    int nextFrame = getNextKeyFramePosition(frameNumber);
    Camera* camera2 = static_cast<Camera*>(getLastKeyFrameAtPosition(nextFrame));

    if (camera1 == NULL && camera2 == NULL)
    {
        return; // do nothing
    }
    else if (camera1 == NULL && camera2 != NULL)
    {
        return cam->assign(*camera2);
    }
    else if (camera2 == NULL && camera1 != NULL)
    {
        return cam->assign(*camera1);
    }

    if (camera1 == camera2)
    {
        return cam->assign(*camera1);
    }

    double frame1 = camera1->pos();
    double frame2 = camera2->pos();

    // linear interpolation
    double c2 = (frameNumber - frame1) / (frame2 - frame1);

    auto lerp = [](double f1, double f2, double ratio) -> double
    {
        return f1 * (1.0 - ratio) + f2 * ratio;
    };

    double dx = lerp(camera1->translation().x(), camera2->translation().x(), c2);
    double dy = lerp(camera1->translation().y(), camera2->translation().y(), c2);
    double r = lerp(camera1->rotation(), camera2->rotation(), c2);
    double s = lerp(camera1->scaling(), camera2->scaling(), c2);

    cam->translate(dx, dy);
    cam->rotate(r);
    cam->scale(s);
}

QRect LayerCamera::getViewRect()
{
    return viewRect;
}

QSize LayerCamera::getViewSize()
{
    return viewRect.size();
}

void LayerCamera::loadImageAtFrame( int frameNumber, float dx, float dy, float rotate, float scale)
{
    if ( keyExists( frameNumber ) )
    {
        removeKeyFrame( frameNumber );
    }
    Camera* camera = new Camera(QPointF(dx, dy), rotate, scale);
    camera->setPos( frameNumber );
    loadKey(camera);
}


Status LayerCamera::saveKeyFrame( KeyFrame* pKeyFrame, QString path )
{
    Q_UNUSED( path );
    Q_UNUSED( pKeyFrame );
    return Status::OK;
}


void LayerCamera::editProperties()
{
    if ( dialog == NULL )
    {
        dialog = new CameraPropertiesDialog( name(), viewRect.width(), viewRect.height() );
    }
    dialog->setName( name() );
    dialog->setWidth(viewRect.width());
    dialog->setHeight(viewRect.height());
    int result = dialog->exec();
    if (result == QDialog::Accepted)
    {
        setName( dialog->getName() );
        viewRect = QRect(-dialog->getWidth()/2, -dialog->getHeight()/2, dialog->getWidth(), dialog->getHeight());

        setUpdated();
    }
}

Status LayerCamera::writeXmlData( QXmlStreamWriter& doc )
{
    if(doc.hasError()) return Status::FAIL;

    doc.writeStartElement("layer");
    
    doc.writeAttribute("name", name());
    doc.writeAttribute("visibility", visible() ? "1" : "0");
    doc.writeAttribute("type", QString::number(type()));
    doc.writeAttribute("width", QString::number(viewRect.width()));
    doc.writeAttribute("height", QString::number(viewRect.height()));

    if(doc.hasError())
    {
        QStringList debugInfo = QStringList() << "LayerCamera::writeXMLData"
                                              << "layer tag"
                                              << QString("name = %1").arg(name())
                                              << QString("visibility = %1").arg(visible())
                                              << QString("type = %1").arg(type())
                                              << QString("width = %1").arg(viewRect.width())
                                              << QString("height = %1").arg(viewRect.height());
        return Status(Status::FAIL, debugInfo);
    }

    QStringList debugInfoKeyFrame;

    foreachKeyFrame( [&]( KeyFrame* pKeyFrame )
    {
        // Don't bother processing the key frame if you can't write it
        if(doc.hasError()) return;

        Camera* camera = static_cast< Camera* >( pKeyFrame );
        doc.writeStartElement("camera");
        doc.writeAttribute( "frame", QString::number(camera->pos()) );

        doc.writeAttribute( "r", QString::number(camera->rotation()) );
        doc.writeAttribute( "s", QString::number(camera->scaling()) );
        doc.writeAttribute( "dx",  QString::number(camera->translation().x()) );
        doc.writeAttribute( "dy",  QString::number(camera->translation().y()) );
        doc.writeEndElement(); // End camera tag

        if(doc.hasError())
        {
            debugInfoKeyFrame << "LayerCamera::writeXMLData"
                              << "camera tag"
                              << QString("pKeyFrame->pos() = %1").arg(pKeyFrame->pos())
                              << QString("r = %1").arg(camera->rotation())
                              << QString("s = %1").arg(camera->scaling())
                              << QString("dx = %1").arg(camera->translation().x())
                              << QString("dy = %1").arg(camera->translation().y());
        }
    } );

    if(!debugInfoKeyFrame.empty()) {
        return Status(Status::FAIL, debugInfoKeyFrame);
    }
    
    doc.writeEndElement(); // End layer tag

    if(doc.hasError())
    {
        QStringList debugInfo = QStringList() << "LayerCamera::writeXMLData"
                                              << "layer tag end"
                                              << QString("name = %1").arg(name())
                                              << QString("visibility = %1").arg(visible())
                                              << QString("type = %1").arg(type())
                                              << QString("width = %1").arg(viewRect.width())
                                              << QString("height = %1").arg(viewRect.height());
        return Status(Status::FAIL, debugInfo);
    }

    return Status::OK;
}

void LayerCamera::loadDomElement(QDomElement element, QString dataDirPath)
{
    Q_UNUSED(dataDirPath);

    setName( element.attribute("name") );
    setVisible( true );

    int width = element.attribute( "width" ).toInt();
    int height = element.attribute( "height" ).toInt();
    viewRect = QRect( -width / 2, -height / 2, width, height );

    QDomNode imageTag = element.firstChild();
    while (!imageTag.isNull())
    {
        QDomElement imageElement = imageTag.toElement();
        if (!imageElement.isNull())
        {
            if (imageElement.tagName() == "camera")
            {
                int frame = imageElement.attribute("frame").toInt();

                qreal rotate = imageElement.attribute("r", "0").toDouble();
                qreal scale = imageElement.attribute("s", "1").toDouble();
                qreal dx = imageElement.attribute("dx", "0").toDouble();
                qreal dy = imageElement.attribute("dy", "0").toDouble();

                loadImageAtFrame( frame, dx, dy, rotate, scale );
            }
        }
        imageTag = imageTag.nextSibling();
    }
}
