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
