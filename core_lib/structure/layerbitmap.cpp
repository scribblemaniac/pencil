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
#include "layerbitmap.h"

#include <QDebug>
#include "keyframe.h"
#include "bitmapimage.h"



LayerBitmap::LayerBitmap(Object* object) : Layer(object, Layer::BITMAP)
{
    setName(tr("Bitmap Layer"));
}

LayerBitmap::~LayerBitmap()
{
}

BitmapImage* LayerBitmap::getBitmapImageAtFrame(int frameNumber)
{
    Q_ASSERT(frameNumber >= 1);
    return static_cast<BitmapImage*>(getKeyFrameAt(frameNumber));
}

BitmapImage* LayerBitmap::getLastBitmapImageAtFrame(int frameNumber, int increment)
{
    Q_ASSERT(frameNumber >= 1);
    return static_cast<BitmapImage*>(getLastKeyFrameAtPosition(frameNumber + increment));
}

void LayerBitmap::loadImageAtFrame(QString path, QPoint topLeft, int frameNumber)
{
    BitmapImage* pKeyFrame = new BitmapImage(path, topLeft);
    pKeyFrame->setPos(frameNumber);
    loadKey(pKeyFrame);
}

Status LayerBitmap::saveKeyFrame(KeyFrame* pKeyFrame, QString path)
{
    QStringList debugInfo = QStringList() << "LayerBitmap::saveKeyFrame" << QString("pKeyFrame.pos() = %1").arg(pKeyFrame->pos()) << QString("path = %1").arg(path);
    BitmapImage* pBitmapImage = static_cast<BitmapImage*>(pKeyFrame);

    QString theFileName = fileName(pKeyFrame->pos());
    QString strFilePath = QDir(path).filePath(theFileName);
    debugInfo << QString("strFilePath = ").arg(strFilePath);
    if (!pBitmapImage->image()->save(strFilePath) && !pBitmapImage->image()->isNull())
    {
        return Status(Status::FAIL, debugInfo << QString("pBitmapImage could not be saved"));
    }

    return Status::OK;
}

QString LayerBitmap::fileName(int frame) const
{
    QString layerNumberString = QString::number(id());
    QString frameNumberString = QString::number(frame);
    while (layerNumberString.length() < 3) layerNumberString.prepend("0");
    while (frameNumberString.length() < 3) frameNumberString.prepend("0");
    return layerNumberString + "." + frameNumberString + ".png";
}

Status LayerBitmap::writeXmlData( QXmlStreamWriter& doc )
{
    if(doc.hasError()) return Status::FAIL;

    doc.writeStartElement("layer");

    doc.writeAttribute("id", QString::number(id()));
    doc.writeAttribute("name", name());
    doc.writeAttribute("visibility", visible() ? "1" : "0");
    doc.writeAttribute("type", QString::number(type()));

    if(doc.hasError())
    {
        QStringList debugInfo = QStringList() << "LayerBitmap::writeXMLData"
                                              << "layer tag"
                                              << QString("id = %1").arg(id())
                                              << QString("name = %1").arg(name())
                                              << QString("visibility = %1").arg(visible())
                                              << QString("type = %1").arg(type());
        return Status(Status::FAIL, debugInfo);
    }

    QStringList debugInfoKeyFrame;

    foreachKeyFrame([&](KeyFrame* pKeyFrame) // TODO: use something else
    {
        // Don't bother processing the key frame if you can't write it
        if(doc.hasError()) return;

        BitmapImage* pImg = static_cast<BitmapImage*>(pKeyFrame);

        doc.writeStartElement("image");
        doc.writeAttribute("frame", QString::number(pKeyFrame->pos()));
        doc.writeAttribute("src", fileName(pKeyFrame->pos()));
        doc.writeAttribute("topLeftX", QString::number(pImg->topLeft().x()));
        doc.writeAttribute("topLeftY", QString::number(pImg->topLeft().y()));
        doc.writeEndElement(); // End image tag

        if(doc.hasError())
        {
            debugInfoKeyFrame << "LayerBitmap::writeXMLData"
                              << "image tag"
                              << QString("frame = %1").arg(pKeyFrame->pos())
                              << QString("src = %1").arg(fileName(pKeyFrame->pos()))
                              << QString("topLeftX = %1").arg(pImg->topLeft().x())
                              << QString("topLeftY = %1").arg(pImg->topLeft().y());
        }
    });

    if(!debugInfoKeyFrame.empty()) {
        return Status(Status::FAIL, debugInfoKeyFrame);
    }

    doc.writeEndElement(); // End layer tag

    if(doc.hasError())
    {
        QStringList debugInfo = QStringList() << "LayerBitmap::writeXMLData"
                                              << "layer tag end"
                                              << QString("id = %1").arg(id())
                                              << QString("name = %1").arg(name())
                                              << QString("visibility = %1").arg(visible())
                                              << QString("type = %1").arg(type());
        return Status(Status::FAIL, debugInfo);
    }

    return Status::OK;
}

void LayerBitmap::loadDomElement(QDomElement element, QString dataDirPath)
{
    if (!element.attribute("id").isNull())
    {
        int id = element.attribute("id").toInt();
        setId(id);
    }
    setName(element.attribute("name"));
    setVisible(element.attribute("visibility").toInt() == 1);

    QDomNode imageTag = element.firstChild();
    while (!imageTag.isNull())
    {
        QDomElement imageElement = imageTag.toElement();
        if (!imageElement.isNull())
        {
            if (imageElement.tagName() == "image")
            {
                QString path = dataDirPath + "/" + imageElement.attribute("src"); // the file is supposed to be in the data directory
                //qDebug() << "LAY_BITMAP  dataDirPath=" << dataDirPath << "   ;path=" << path;  //added for debugging puproses
                QFileInfo fi(path);
                if (!fi.exists()) path = imageElement.attribute("src");
                int position = imageElement.attribute("frame").toInt();
                int x = imageElement.attribute("topLeftX").toInt();
                int y = imageElement.attribute("topLeftY").toInt();
                loadImageAtFrame(path, QPoint(x, y), position);
            }
        }
        imageTag = imageTag.nextSibling();
    }
}
