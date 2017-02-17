QT += core gui svg

CONFIG += c++11

TARGET = Benchmark
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    Other.cpp \
    Svg.cpp \
    Png.cpp \
    Bmp.cpp

INCLUDEPATH += /usr/local/include/hayai

LIBS += -L/usr/local/lib -lhayai_main

RESOURCES += \
    resources.qrc
