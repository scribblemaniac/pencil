/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#ifndef BACKUPELEMENT_H
#define BACKUPELEMENT_H

#include <QObject>
#include <QUndoCommand>
#include <QRectF>
#include <QTransform>

#include "movemode.h"
#include "pencildef.h"
#include "layer.h"
#include "vectorselection.h"
#include "preferencemanager.h"

class Editor;
class BackupManager;
class PreferenceManager;
class BitmapImage;
class VectorImage;
class SoundClip;
class Camera;
class Layer;
class KeyFrame;

enum types { UNDEFINED,
             ADD_KEY_MODIF,
             REMOVE_KEY_MODIF
           };

class BackupElement : public QUndoCommand
{
public:
    explicit BackupElement(Editor* editor, QUndoCommand* parent = nullptr);
    virtual ~BackupElement();

    Editor* editor() { return mEditor; }

    virtual int type() const { return UNDEFINED; }
    void undo() override;
    void redo() override;

    virtual void applyUndo() = 0;
    virtual void applyRedo() = 0;
private:
    Editor* mEditor = nullptr;
    bool mIsFirstRedo = true;
};

class FlipViewElement : public BackupElement
{
public:
    FlipViewElement(const bool backupFlipEnabled,
                    const FlipDirection backupFlipDirection,
                    Editor* editor,
                    QUndoCommand* parent = nullptr);

    bool isFlipped = false;

    FlipDirection direction;

    void applyUndo() override;
    void applyRedo() override;
};

class MoveLayerElement : public BackupElement
{

public:
    MoveLayerElement(const int backupOldLayerIndex,
                     const int backupNewLayerIndex,
                     Editor* editor,
                     QUndoCommand* parent = nullptr);

    int oldLayerIndex = 0;
    int newLayerIndex = 0;

    void applyUndo() override;
    void applyRedo() override;
};

#endif // BACKUPELEMENT_H
