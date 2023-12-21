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

BackupElement::BackupElement(Editor* editor, QUndoCommand* parent) :
    QUndoCommand(parent), mEditor(editor)
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
                                 QUndoCommand *parent) : BackupElement(editor, parent)
{
    isFlipped = backupFlipState;
    direction = backupFlipDirection;

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
