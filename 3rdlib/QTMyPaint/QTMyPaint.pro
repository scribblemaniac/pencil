#-------------------------------------------------
#
# Project created by QtCreator 2015-03-31T23:25:53
#
#-------------------------------------------------
TEMPLATE = subdirs

json-c.subdir = json-c

libmypaint.subdir = libmypaint
libmypaint.depends = json-c

SUBDIRS = json-c \
          libmypaint \
          src

INCLUDEPATH += $$PWD
