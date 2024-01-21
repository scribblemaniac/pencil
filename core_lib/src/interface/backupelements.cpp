/*

Pencil - Traditional Animation Software
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

#include "backupelements.h"

#include <QCoreApplication>
#include "QProgressDialog"
#include <QDebug>

#include "layermanager.h"
#include "viewmanager.h"
#include "selectionmanager.h"

#include "layersound.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "layercamera.h"

#include "editor.h"

#include "vectorimage.h"
#include "bitmapimage.h"
#include "soundclip.h"
#include "camera.h"

BackupElement::BackupElement(Editor* editor, QUndoCommand* parent) : QUndoCommand(parent),
    mEditor(editor)
{
}

BackupElement::~BackupElement()
{
}

void BackupElement::undo()
{
    applyUndo();
}

void BackupElement::redo()
{
    if (mIsFirstRedo)
    {
        mIsFirstRedo = false;
        return;
    }

    applyRedo();
}

FlipViewElement::FlipViewElement(const bool backupFlipState,
                                 const FlipDirection backupFlipDirection,
                                 Editor *editor,
                                 QUndoCommand *parent) : BackupElement(editor, parent),
    isFlipped(backupFlipState),
    direction(backupFlipDirection)
{
    switch (direction)
    {
    case FlipDirection::HORIZONTAL:
        setText(QObject::tr("Flip View X"));
        break;
    case FlipDirection::VERTICAL:
        setText(QObject::tr("Flip View Y"));
        break;
    default:
        Q_UNREACHABLE();
    }
}

void FlipViewElement::applyUndo()
{
    switch (direction)
    {
    case FlipDirection::HORIZONTAL:
        editor()->view()->flipHorizontal(!isFlipped);
        break;
    case FlipDirection::VERTICAL:
        editor()->view()->flipVertical(!isFlipped);
        break;
    default:
        Q_UNREACHABLE();
    }
}

void FlipViewElement::applyRedo()
{
    switch (direction)
    {
    case FlipDirection::HORIZONTAL:
        editor()->view()->flipHorizontal(isFlipped);
        break;
    case FlipDirection::VERTICAL:
        editor()->view()->flipVertical(isFlipped);
        break;
    default:
        Q_UNREACHABLE();
    }
}

MoveLayerElement::MoveLayerElement(const int backupOldLayerIndex,
                                   const int backupNewLayerIndex,
                                   Editor* editor,
                                   QUndoCommand* parent) : BackupElement(editor, parent),
    oldLayerIndex(backupOldLayerIndex),
    newLayerIndex(backupNewLayerIndex)
{
    setText(QObject::tr("Move layer"));
}

void MoveLayerElement::applyUndo()
{
    editor()->moveLayers(newLayerIndex, oldLayerIndex);
    editor()->layers()->setCurrentLayer(oldLayerIndex);
}

void MoveLayerElement::applyRedo()
{
    editor()->moveLayers(oldLayerIndex, newLayerIndex);
    editor()->layers()->setCurrentLayer(newLayerIndex);
}

MoveFramesElement::MoveFramesElement(const int backupLayerId, const int backupOffset, const int oldFrameIndex, Editor *editor, QUndoCommand *parent)
    : MoveFramesElement(backupLayerId, backupOffset, QList<int>{oldFrameIndex}, editor, parent)
{
}

MoveFramesElement::MoveFramesElement(const int backupLayerId,
                                     const int backupOffset,
                                     const QList<int> oldFrameIndexes,
                                     Editor* editor,
                                     QUndoCommand* parent) : BackupElement(editor, parent),
    layerId(backupLayerId),
    offset(backupOffset),
    oldFrameIndexes(oldFrameIndexes)
{
    if (backupOffset < 0) {
        setText(QObject::tr("Move %n frame(s) backward", "Undo/redo", oldFrameIndexes.size()));
    } else {
        setText(QObject::tr("Move %n frame(s) forward", "Undo/redo", oldFrameIndexes.size()));
    }
}

void MoveFramesElement::applyUndo()
{
    QList<int> newFrameIndexes;
    for (const int frameIndex : oldFrameIndexes)
    {
        newFrameIndexes.append(frameIndex+offset);
    }
    Layer* layer = editor()->layers()->findLayerById(layerId);

    if (oldFrameIndexes.size() == 1)
    {
        moveSingle(newFrameIndexes.first(), -offset, layer);
    }
    else
    {
        moveMultiple(newFrameIndexes, -offset, layer);
    }
    editor()->layers()->setCurrentLayer(layer);

    editor()->layers()->notifyAnimationLengthChanged();
    emit editor()->framesModified();
}

void MoveFramesElement::applyRedo()
{
    Layer* layer = editor()->layers()->findLayerById(layerId);

    if (oldFrameIndexes.size() == 1)
    {
        moveSingle(oldFrameIndexes.first(), offset, layer);
    }
    else
    {
        moveMultiple(oldFrameIndexes, offset, layer);
    }
    editor()->layers()->setCurrentLayer(layer);

    editor()->layers()->notifyAnimationLengthChanged();
    emit editor()->framesModified();
}

void MoveFramesElement::moveSingle(const int oldFrameIndex, const int offset, Layer* layer)
{
    layer->moveKeyFrame(oldFrameIndex, offset);
    editor()->scrubTo(oldFrameIndex+offset);
}

void MoveFramesElement::moveMultiple(const QList<int> oldFrameIndexes, const int offset, Layer *layer)
{
    layer->deselectAll();
    layer->setFramesSelected(oldFrameIndexes);
    layer->moveSelectedFrames(offset);
}
