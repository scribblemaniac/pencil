######################################################################
# Automatically generated by qmake (2.01a) sam. janv. 13 17:20:35 2007
######################################################################

TEMPLATE = subdirs

# build the project sequentially as listed in SUBDIRS !
CONFIG += ordered

SUBDIRS += core_lib
core_lib.subdir = core_lib

SUBDIRS += app
app.subdir = app
app.depends = core_lib

SUBDIRS += tests
tests.subdir = tests
tests.depends = core_lib

SUBDIRS += benchmarks
benchmarks.subdir = benchmarks
benchmarks.depends = core_lib

NO_TESTS {
  SUBDIRS -= tests
}

TRANSLATIONS += $$PWD/translations/pencil.ts

