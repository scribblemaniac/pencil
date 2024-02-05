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
#ifndef SelectionPainter_H
#define SelectionPainter_H

#include <QRectF>
#include <QPolygonF>
#include <QTransform>

#include "baseframepainter.h"

class QPainter;
class Object;
class BaseTool;

struct SelectionPainterOptions
{
    QTransform viewTransform;
    QTransform selectionTransform;
    QRectF selectionRect;
};

class SelectionPainter: public BaseFramePainter
{
public:
    SelectionPainter();

    void paint(QPainter& painter, const Object* object, int layerIndex, BaseTool* tool);
    void paintBitmapFrame(QPainter& painter, const QRect& blitRect, BitmapImage* bitmapImage) override;
    void paintVectorFrame(QPainter& painter, const QRect& blitRect, VectorImage* vectorImage) override;
    bool isBitmapModifierActive(bool isCurrentLayer) const override;
    bool isVectorModifierActive(bool isCurrentLayer) const override;

    void setPainterOptions(const SelectionPainterOptions& options) { mOptions = options; }

private:
    void paintSelectionInfo(QPainter& painter, const QTransform& mergedTransform, const QTransform& viewTransform, const QRectF& selectionRect, const QPolygonF& projectedPolygonF);

    SelectionPainterOptions mOptions;
    const static int HANDLE_WIDTH = 6;
};

#endif // SelectionPainter_H
