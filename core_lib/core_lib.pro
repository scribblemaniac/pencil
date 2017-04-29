#-------------------------------------------------
#
# Pencil2D core library
#
#-------------------------------------------------

! include( ../common.pri ) { error( Could not find the common.pri file! ) }

QT += core widgets gui xml xmlpatterns multimedia svg

TEMPLATE = lib
CONFIG += qt staticlib console

RESOURCES += ../pencil.qrc

MOC_DIR = .moc
OBJECTS_DIR = .obj


INCLUDEPATH += . \
    graphics \
    graphics/bitmap \
    graphics/vector \
    interface \
    structure \
    tool \
    util \
    ui \
    managers

# Input
HEADERS +=  \
    graphics/bitmap/bitmapimage.h \
    graphics/vector/bezierarea.h \
    graphics/vector/beziercurve.h \
    graphics/vector/colourref.h \
    graphics/vector/vectorimage.h \
    graphics/vector/vectorselection.h \
    graphics/vector/vertexref.h \
    interface/backupelement.h \
    interface/editor.h \
    interface/flowlayout.h \
    interface/keycapturelineedit.h \
    interface/recentfilemenu.h \
    interface/scribblearea.h \
    interface/spinslider.h \
    interface/timecontrols.h \
    interface/timeline.h \
    interface/timelinecells.h \
    interface/toolbox.h \
    interface/tooloptiondockwidget.h \
    managers/basemanager.h \
    managers/colormanager.h \
    managers/layermanager.h \
    managers/toolmanager.h \
    managers/playbackmanager.h \
    managers/viewmanager.h \
    structure/camera.h \
    structure/keyframe.h \
    structure/layer.h \
    structure/layerbitmap.h \
    structure/layercamera.h \
    structure/layersound.h \
    structure/layervector.h \
    structure/object.h \
    tool/basetool.h \
    tool/brushtool.h \
    tool/buckettool.h \
    tool/erasertool.h \
    tool/eyedroppertool.h \
    tool/handtool.h \
    tool/movetool.h \
    tool/penciltool.h \
    tool/pentool.h \
    tool/polylinetool.h \
    tool/selecttool.h \
    tool/smudgetool.h \
    tool/strokemanager.h \
    tool/stroketool.h \
    util/blitrect.h \
    util/fileformat.h \
    util/pencildef.h \
    util/pencilerror.h \
    util/pencilsettings.h \
    util/util.h \
    interface/basedockwidget.h \
    util/log.h \
    canvasrenderer.h \
    managers/preferencemanager.h \
    soundplayer.h \
    structure/keyframefactory.h \
    structure/soundclip.h \
    managers/soundmanager.h \
    interface/backgroundwidget.h \
    structure/editorstate.h \
    structure/filemanager.h \
    interface/layerlabel.h \
    interface/layerlabeledit.h \
    interface/tracklist.h \
    interface/layericon.h


SOURCES +=  graphics/bitmap/bitmapimage.cpp \
    graphics/vector/bezierarea.cpp \
    graphics/vector/beziercurve.cpp \
    graphics/vector/colourref.cpp \
    graphics/vector/vectorimage.cpp \
    graphics/vector/vectorselection.cpp \
    graphics/vector/vertexref.cpp \
    interface/backupelement.cpp \
    interface/editor.cpp \
    interface/flowlayout.cpp \
    interface/keycapturelineedit.cpp \
    interface/recentfilemenu.cpp \
    interface/scribblearea.cpp \
    interface/spinslider.cpp \
    interface/timecontrols.cpp \
    interface/timeline.cpp \
    interface/timelinecells.cpp \
    interface/toolbox.cpp \
    interface/tooloptiondockwidget.cpp \
    managers/basemanager.cpp \
    managers/colormanager.cpp \
    managers/layermanager.cpp \
    managers/toolmanager.cpp \
    structure/camera.cpp \
    structure/keyframe.cpp \
    structure/layer.cpp \
    structure/layerbitmap.cpp \
    structure/layercamera.cpp \
    structure/layersound.cpp \
    structure/layervector.cpp \
    structure/object.cpp \
    tool/basetool.cpp \
    tool/brushtool.cpp \
    tool/buckettool.cpp \
    tool/erasertool.cpp \
    tool/eyedroppertool.cpp \
    tool/handtool.cpp \
    tool/movetool.cpp \
    tool/penciltool.cpp \
    tool/pentool.cpp \
    tool/polylinetool.cpp \
    tool/selecttool.cpp \
    tool/smudgetool.cpp \
    tool/strokemanager.cpp \
    tool/stroketool.cpp \
    util/blitrect.cpp \
    util/fileformat.cpp \
    util/pencilerror.cpp \
    util/pencilsettings.cpp \
    interface/basedockwidget.cpp \
    managers/playbackmanager.cpp \
    managers/viewmanager.cpp \
    util/util.cpp \
    canvasrenderer.cpp \
    managers/preferencemanager.cpp \
    soundplayer.cpp \
    structure/keyframefactory.cpp \
    structure/soundclip.cpp \
    managers/soundmanager.cpp \
    interface/backgroundwidget.cpp \
    structure/editorstate.cpp \
    structure/filemanager.cpp \
    interface/layerlabeledit.cpp \
    interface/tracklist.cpp \
    interface/layericon.cpp

win32 {
    CONFIG -= flat
	CONFIG += grouped

    INCLUDEPATH += external/win32
    SOURCES += external/win32/win32.cpp
}

macx {
    INCLUDEPATH += external/macosx
    SOURCES += external/macosx/macosx.cpp
}

unix:!macx {
    INCLUDEPATH += external/linux
    SOURCES += external/linux/linux.cpp
}


INCLUDEPATH += $$PWD/../3rdlib/quazip
DEPENDPATH += $$PWD/../3rdlib/quazip

INCLUDEPATH += $$PWD/../3rdlib/zlib
