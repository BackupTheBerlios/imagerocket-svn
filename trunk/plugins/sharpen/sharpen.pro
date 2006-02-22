TEMPLATE = lib
CONFIG += plugin

HEADERS = sharpen.h
SOURCES = sharpen.cpp
FORMS = sharpenwidget.ui
TARGET = sharpen
DESTDIR = .
INCLUDEPATH += ../../app
CONFIG += qt warn_on
CONFIG -= debug_and_release

