TEMPLATE = lib
CONFIG += plugin

HEADERS = bc.h
SOURCES = bc.cpp
FORMS = bcwidget.ui
TARGET = bc
DESTDIR = .
INCLUDEPATH += ../../app ../../lua
CONFIG += qt warn_on
CONFIG -= debug_and_release

