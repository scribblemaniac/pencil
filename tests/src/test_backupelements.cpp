/*

Pencil2D - Traditional Animation Software
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include "catch.hpp"

#include "backupelements.h"
#include "editor.h"
#include "object.h"
#include "scribblearea.h"
#include "viewmanager.h"
#include "layermanager.h"
#include "layerbitmap.h"

TEST_CASE("FlipViewElement")
{
    SECTION("Init")
    {
        SECTION("Horizontal unflipped -> flipped")
        {
            FlipViewElement elem(true, FlipDirection::HORIZONTAL, nullptr);

            REQUIRE(!elem.text().isEmpty());
        }

        SECTION("Horizontal flipped -> unflipped")
        {
            FlipViewElement elem(false, FlipDirection::HORIZONTAL, nullptr);

            REQUIRE(!elem.text().isEmpty());
        }

        SECTION("Vertical unflipped -> flipped")
        {
            FlipViewElement elem(true, FlipDirection::VERTICAL, nullptr);

            REQUIRE(!elem.text().isEmpty());
        }

        SECTION("Vertical flipped -> unflipped")
        {
            FlipViewElement elem(false, FlipDirection::VERTICAL, nullptr);

            REQUIRE(!elem.text().isEmpty());
        }
    }

    // Setup editor
    std::unique_ptr<Editor> editor(new Editor);
    ScribbleArea scribbleArea(nullptr);
    editor->setScribbleArea(&scribbleArea);
    editor->setObject(new Object);
    editor->init();

    SECTION("Undo")
    {
        SECTION("Horizontal unflipped -> flipped")
        {
            editor->view()->flipHorizontal(true);
            REQUIRE(editor->view()->isFlipHorizontal() == true);
            FlipViewElement elem(true, FlipDirection::HORIZONTAL, editor.get());
            elem.applyUndo();

            REQUIRE(editor->view()->isFlipHorizontal() == false);
        }

        SECTION("Horizontal flipped -> unflipped")
        {
            editor->view()->flipHorizontal(false);
            REQUIRE(editor->view()->isFlipHorizontal() == false);
            FlipViewElement elem(false, FlipDirection::HORIZONTAL, editor.get());
            elem.applyUndo();

            REQUIRE(editor->view()->isFlipHorizontal() == true);
        }

        SECTION("Vertical unflipped -> flipped")
        {
            editor->view()->flipVertical(true);
            REQUIRE(editor->view()->isFlipVertical() == true);
            FlipViewElement elem(true, FlipDirection::VERTICAL, editor.get());
            elem.applyUndo();

            REQUIRE(editor->view()->isFlipVertical() == false);
        }

        SECTION("Vertical flipped -> unflipped")
        {
            editor->view()->flipVertical(false);
            REQUIRE(editor->view()->isFlipVertical() == false);
            FlipViewElement elem(false, FlipDirection::VERTICAL, editor.get());
            elem.applyUndo();

            REQUIRE(editor->view()->isFlipVertical() == true);
        }
    }

    SECTION("Redo")
    {
        SECTION("Horizontal unflipped -> flipped")
        {
            editor->view()->flipHorizontal(false);
            REQUIRE(editor->view()->isFlipHorizontal() == false);
            FlipViewElement elem(true, FlipDirection::HORIZONTAL, editor.get());
            elem.applyRedo();

            REQUIRE(editor->view()->isFlipHorizontal() == true);
        }

        SECTION("Horizontal flipped -> unflipped")
        {
            editor->view()->flipHorizontal(true);
            REQUIRE(editor->view()->isFlipHorizontal() == true);
            FlipViewElement elem(false, FlipDirection::HORIZONTAL, editor.get());
            elem.applyRedo();

            REQUIRE(editor->view()->isFlipHorizontal() == false);
        }

        SECTION("Vertical unflipped -> flipped")
        {
            editor->view()->flipVertical(false);
            REQUIRE(editor->view()->isFlipVertical() == false);
            FlipViewElement elem(true, FlipDirection::VERTICAL, editor.get());
            elem.applyRedo();

            REQUIRE(editor->view()->isFlipVertical() == true);
        }

        SECTION("Vertical flipped -> unflipped")
        {
            editor->view()->flipVertical(true);
            REQUIRE(editor->view()->isFlipVertical() == true);
            FlipViewElement elem(false, FlipDirection::VERTICAL, editor.get());
            elem.applyRedo();

            REQUIRE(editor->view()->isFlipVertical() == false);
        }
    }
}

TEST_CASE("MoveLayerElement")
{
    const int LAYER_COUNT = 3;

    // Setup editor
    std::unique_ptr<Editor> editor(new Editor);
    ScribbleArea scribbleArea(nullptr);
    editor->setScribbleArea(&scribbleArea);
    editor->setObject(new Object);
    editor->init();
    scribbleArea.setEditor(editor.get());

    // Add layers
    std::array<Layer*, LAYER_COUNT> movableLayers;
    for (int i = 0; i < LAYER_COUNT; i++) {
        movableLayers[i] = static_cast<Layer*>(editor->object()->addNewBitmapLayer());
    }
    editor->object()->addNewCameraLayer();

    SECTION("Init")
    {
        MoveLayerElement elem(0, 1, editor.get());

        REQUIRE(!elem.text().isEmpty());
    }

    struct MoveLayerTestArgs
    {
        MoveLayerTestArgs(const int oi, const int ni, std::array<int, LAYER_COUNT> conf) :
            oldLayerIndex(oi), newLayerIndex(ni), finalConfiguration(conf) {}

        int oldLayerIndex, newLayerIndex;
        std::array<int, LAYER_COUNT> finalConfiguration;
    };

    for (int i = 0; i < LAYER_COUNT; i++) {
        REQUIRE(editor->layers()->getLayer(i) == movableLayers[i]);
    }

    SECTION("Undo")
    {
        MoveLayerTestArgs args = GENERATE(values<MoveLayerTestArgs>({
            {0, 1, {1,0,2}},
            {0, 2, {2,0,1}},
            {1, 0, {1,0,2}},
            {1, 2, {0,2,1}},
            {2, 0, {1,2,0}},
            {2, 1, {0,2,1}}
        }));
        int selectedIndex = GENERATE(0,1,2);
        editor->setCurrentLayerIndex(selectedIndex);
        REQUIRE(editor->currentLayerIndex() == selectedIndex);

        MoveLayerElement elem(args.oldLayerIndex, args.newLayerIndex, editor.get());
        elem.applyUndo();

        for (int i = 0; i < LAYER_COUNT; i++) {
            REQUIRE(editor->layers()->getLayer(i) == movableLayers[args.finalConfiguration[i]]);
        }
        REQUIRE(editor->currentLayerIndex() == args.oldLayerIndex);
    }

    SECTION("Redo")
    {
        MoveLayerTestArgs args = GENERATE(values<MoveLayerTestArgs>({
            {0, 1, {1,0,2}},
            {0, 2, {1,2,0}},
            {1, 0, {1,0,2}},
            {1, 2, {0,2,1}},
            {2, 0, {2,0,1}},
            {2, 1, {0,2,1}}
        }));
        int selectedIndex = GENERATE(0,1,2);
        editor->setCurrentLayerIndex(selectedIndex);
        REQUIRE(editor->currentLayerIndex() == selectedIndex);

        MoveLayerElement elem(args.oldLayerIndex, args.newLayerIndex, editor.get());
        elem.applyRedo();

        for (int i = 0; i < LAYER_COUNT; i++) {
            REQUIRE(editor->layers()->getLayer(i) == movableLayers[args.finalConfiguration[i]]);
        }
        REQUIRE(editor->currentLayerIndex() == args.newLayerIndex);
    }
}

void checkFramesMoved(const QList<int> expectedPositions, const Layer* layer, const QList<KeyFrame*> keyframes)
{
    REQUIRE(layer->keyFrameCount() == expectedPositions.size());
    for (int i = 0; i < expectedPositions.size(); i++)
    {
        REQUIRE(layer->getKeyFrameAt(expectedPositions[i]) == keyframes[i]);
    }
}

TEST_CASE("MoveFramesElement")
{
    // Setup editor
    std::unique_ptr<Editor> editor(new Editor);
    ScribbleArea scribbleArea(nullptr);
    editor->setScribbleArea(&scribbleArea);
    editor->setObject(new Object);
    editor->init();
    scribbleArea.setEditor(editor.get());

    // Add layer 0 with keyframes at positions 1,3,4,6
    LayerBitmap* layer = editor->object()->addNewBitmapLayer();
    int layerId = layer->id();
    QList<KeyFrame*> keyframes;
    QList<int> expectedPositions{1,3,4,6};
    for (int pos : expectedPositions)
    {
        layer->addNewKeyFrameAt(pos);
        KeyFrame* key = layer->getKeyFrameAt(pos);
        REQUIRE(key != nullptr);
        keyframes.append(key);
    }
    layer->deselectAll();
    layer->setFramesSelected({3,4}, true);
    editor->init();

    SECTION("Init")
    {
        SECTION("Move single frame forward")
        {
            MoveFramesElement elem(layerId, 1, 2, editor.get());

            REQUIRE(!elem.text().isEmpty());
        }

        SECTION("Move single frame backwards")
        {
            MoveFramesElement elem(layerId, -1, 2, editor.get());

            REQUIRE(!elem.text().isEmpty());
        }

        SECTION("Move multiple frames forward")
        {
            MoveFramesElement elem(layerId, 1, {2, 4}, editor.get());

            REQUIRE(!elem.text().isEmpty());
        }

        SECTION("Move multiple frames backwards")
        {
            MoveFramesElement elem(layerId, -1, {2, 4}, editor.get());

            REQUIRE(!elem.text().isEmpty());
        }
    }

    SECTION("Undo")
    {
        SECTION("Move single frame")
        {
            auto args = GENERATE(table<int, int, QList<int>>({
                {1, 2, {1,2,4,6}}, // Move forward 1, dst does not exist
                {1, 3, {1,4,3,6}}, // Move forward 1, dst exists
                {2, 2, {1,3,2,6}}, // Move forward >1, dst does not exist
                {2, 1, {3,1,4,6}}, // Move forward >1, dst exists
                {-1, 5, {1,3,5,6}}, // Move backwards 1, dst does not exist
                {-1, 4, {1,4,3,6}}, // Move backwards 1, dst exists
                {-2, 5, {1,5,4,6}}, // Move backwards >1, dst does not exist
                {-2, 6, {1,3,6,4}}  // Move backwards >1, dst exists
            }));
            int offset = std::get<0>(args);
            int oldPos = std::get<1>(args);
            expectedPositions = std::get<2>(args);
            MoveFramesElement elem(layerId, offset, oldPos, editor.get());
            elem.applyUndo();

            checkFramesMoved(expectedPositions, layer, keyframes);
            REQUIRE(editor->currentFrame() == oldPos);
            // TODO check frame selection
        }

        SECTION("Move multiple frames")
        {
            auto args = GENERATE(table<int, QList<int>, QList<int>>({
                {1, {2,5}, {1,2,4,5}}, // Move forward 1, no overlap
                {1, {2,3}, {1,2,3,6}}, // Move forward 1, overlap
                                       // Move forward >1, no overlap not possible with starting keyframe configuration
                {2, {2,4}, {1,3,2,4}}, // Move forward >1, overlap
                {-1, {2,5}, {2,3,5,6}}, // Move backwards 1, no overlap
                {-1, {4,5}, {1,4,5,6}}, // Move backwards 1, overlap
                {-2, {5,8}, {1,5,4,8}}, // Move backwards >1, no overlap
                {-2, {3,5}, {3,5,4,6}}  // Move backwards >1, overlap
            }));

            int offset = std::get<0>(args);
            QList<int> oldPositions = std::get<1>(args);
            expectedPositions = std::get<2>(args);
            MoveFramesElement elem(layerId, offset, oldPositions, editor.get());
            elem.applyUndo();

            checkFramesMoved(expectedPositions, layer, keyframes);
            REQUIRE(layer->selectedKeyFramesPositions() == oldPositions);
            for (KeyFrame* key : keyframes) {
                REQUIRE(key->isSelected() == oldPositions.contains(key->pos()));
            }
        }
    }

    SECTION("Redo")
    {
        SECTION("Move single frame")
        {
            auto args = GENERATE(table<int, int, QList<int>>({
                {1, 4, {1,3,5,6}}, // Move forward 1, dst does not exist
                {1, 3, {1,4,3,6}}, // Move forward 1, dst exists
                {2, 3, {1,5,4,6}}, // Move forward >1, dst does not exist
                {2, 1, {3,1,4,6}}, // Move forward >1, dst exists
                {-1, 3, {1,2,4,6}}, // Move backwards 1, dst does not exist
                {-1, 4, {1,4,3,6}}, // Move backwards 1, dst exists
                {-2, 4, {1,3,2,6}}, // Move backwards >1, dst does not exist
                {-2, 6, {1,3,6,4}}  // Move backwards >1, dst exists
            }));
            int offset = std::get<0>(args);
            int oldPos = std::get<1>(args);
            expectedPositions = std::get<2>(args);
            MoveFramesElement elem(layerId, offset, oldPos, editor.get());
            elem.applyRedo();

            checkFramesMoved(expectedPositions, layer, keyframes);
            REQUIRE(editor->currentFrame() == oldPos+offset);
            // TOOD check frame selection
        }

        SECTION("Move multiple frames")
        {
            auto args = GENERATE(table<int, QList<int>, QList<int>>({
                {1, {4,6}, {1,3,5,7}}, // Move forward 1, no overlap
                {1, {3,4}, {1,4,5,6}}, // Move forward 1, overlap
                {2, {3,6}, {1,5,4,8}}, // Move forward >1, no overlap
                {2, {4,6}, {1,3,6,8}}, // Move forward >1, overlap
                {-1, {3,6}, {1,2,4,5}}, // Move backwards 1, no overlap
                {-1, {3,4}, {1,2,3,6}}, // Move backwards 1, overlap
                                        // Move backwards >1, no overlap not possible with starting keyframe configuration
                {-2, {4,6}, {1,3,2,4}}  // Move backwards >1, overlap
            }));

            int offset = std::get<0>(args);
            QList<int> oldPositions = std::get<1>(args);
            expectedPositions = std::get<2>(args);
            MoveFramesElement elem(layerId, offset, oldPositions, editor.get());
            elem.applyRedo();

            checkFramesMoved(expectedPositions, layer, keyframes);
            QList<int> newPositions;
            for (int oldPos : oldPositions) {
                newPositions.push_back(oldPos+offset);
            }
            REQUIRE(layer->selectedKeyFramesPositions() == newPositions);
            for (KeyFrame* key : keyframes) {
                REQUIRE(key->isSelected() == newPositions.contains(key->pos()));
            }
        }
    }
}
