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
#include "layersound.h"

#include <QtDebug>
#include <QMediaPlayer>
#include "object.h"
#include "soundclip.h"


LayerSound::LayerSound(Object* object) : Layer(object, Layer::SOUND)
{
    setName(tr("Sound Layer"));
}

LayerSound::~LayerSound()
{
}

Status LayerSound::loadSoundClipAtFrame(const QString& sSoundClipName,
                                        const QString& strFilePath,
                                        int frameNumber)
{
    if (!QFile::exists(strFilePath))
    {
        return Status::FILE_NOT_FOUND;
    }

    QFileInfo info(strFilePath);
    if (!info.isFile())
    {
        return Status::ERROR_LOAD_SOUND_FILE;
    }

    SoundClip* clip = new SoundClip;
    clip->setSoundClipName(sSoundClipName);
    clip->init(strFilePath);
    clip->setPos(frameNumber);
    loadKey(clip);
    return Status::OK;
}

void LayerSound::updateFrameLengths(int fps)
{
    foreachKeyFrame([&fps](KeyFrame* pKeyFrame)
    {
        auto soundClip = dynamic_cast<SoundClip *>(pKeyFrame);
        soundClip->updateLength(fps);
    });
}

Status LayerSound::writeXmlData(QXmlStreamWriter& doc)
{
    if(doc.hasError()) return Status::FAIL;

    doc.writeStartElement("layer");

    doc.writeAttribute("id", QString::number(id()));
    doc.writeAttribute("name", name());
    doc.writeAttribute("visibility", visible() ? "1" : "0");
    doc.writeAttribute("type", QString::number(type()));

    if(doc.hasError())
    {
        QStringList debugInfo = QStringList() << "LayerSound::writeXMLData"
                                              << "layer tag"
                                              << QString("id = %1").arg(id())
                                              << QString("name = %1").arg(name())
                                              << QString("visibility = %1").arg(visible())
                                              << QString("type = %1").arg(type());
        return Status(Status::FAIL, debugInfo);
    }

    QStringList debugInfoKeyFrame;

    foreachKeyFrame([&](KeyFrame* pKeyFrame)
    {
        // Don't bother processing the key frame if you can't write it
        if(doc.hasError()) return;

        SoundClip* clip = static_cast<SoundClip*>(pKeyFrame);

        doc.writeStartElement("sound");
        doc.writeAttribute("frame", QString::number(clip->pos()));
        doc.writeAttribute("name", clip->soundClipName());

        QFileInfo info(clip->fileName());
        //qDebug() << "Save=" << info.fileName();
        doc.writeAttribute("src", info.fileName());
        doc.writeEndElement(); // End sound tag

        if(doc.hasError())
        {
            debugInfoKeyFrame << "LayerSound::writeXMLData"
                              << "sound tag"
                              << QString("frame = %1").arg(clip->pos())
                              << QString("name = %1").arg(clip->soundClipName())
                              << QString("src = %1").arg(info.fileName());
        }
    });

    if(!debugInfoKeyFrame.empty()) {
        return Status(Status::FAIL, debugInfoKeyFrame);
    }

    doc.writeEndElement(); // End layer tag

    if(doc.hasError())
    {
        QStringList debugInfo = QStringList() << "LayerSound::writeXMLData"
                                              << "layer tag end"
                                              << QString("id = %1").arg(id())
                                              << QString("name = %1").arg(name())
                                              << QString("visibility = %1").arg(visible())
                                              << QString("type = %1").arg(type());
        return Status(Status::FAIL, debugInfo);
    }

    return Status::OK;
}

void LayerSound::loadDomElement(QDomElement element, QString dataDirPath)
{
    if (!element.attribute("id").isNull())
    {
        int myId = element.attribute("id").toInt();
        setId(myId);
    }
    setName(element.attribute("name"));
    setVisible(element.attribute("visibility").toInt() == 1);

    QDomNode soundTag = element.firstChild();
    while (!soundTag.isNull())
    {
        QDomElement soundElement = soundTag.toElement();
        if (soundElement.isNull())
        {
            continue;
        }

        if (soundElement.tagName() == "sound")
        {
            const QString soundFile = soundElement.attribute("src");
            const QString sSoundClipName = soundElement.attribute("name", "My Sound Clip");

            // the file is supposed to be in the data directory
            const QString sFullPath = QDir(dataDirPath).filePath(soundFile);

            int position = soundElement.attribute("frame").toInt();
            Status st = loadSoundClipAtFrame(sSoundClipName, sFullPath, position);
            Q_ASSERT(st.ok());
        }

        soundTag = soundTag.nextSibling();
    }
}

Status LayerSound::saveKeyFrame(KeyFrame* key, QString path)
{
    Q_ASSERT(key);

    if (key->fileName().isEmpty())
    {
        return Status::SAFE;
    }

    QFileInfo info(key->fileName());
    QString sDestFileLocation = QDir(path).filePath(info.fileName());

    if (sDestFileLocation != key->fileName())
    {
        bool bOK = QFile::copy(key->fileName(), sDestFileLocation);
        Q_ASSERT(bOK);
        if (!bOK)
        {
            return Status::FAIL;
        }
    }
    return Status::OK;
}

SoundClip* LayerSound::getSoundClipWhichCovers(int frameNumber)
{
    KeyFrame* key = getKeyFrameWhichCovers(frameNumber);
    return static_cast<SoundClip*>(key);
}
