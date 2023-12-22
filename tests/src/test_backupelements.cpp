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
    std::unique_ptr<Editor> editor(new Editor);
    ScribbleArea scribbleArea(nullptr);
    editor->setScribbleArea(&scribbleArea);
    editor->setObject(new Object);
    std::array<Layer*, LAYER_COUNT> movableLayers;
    for (int i = 0; i < LAYER_COUNT; i++) {
        movableLayers[i] = static_cast<Layer*>(editor->object()->addNewBitmapLayer());
    }
    editor->object()->addNewCameraLayer();
    editor->init();
    scribbleArea.setEditor(editor.get());

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
