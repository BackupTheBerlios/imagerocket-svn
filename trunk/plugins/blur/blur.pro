TEMPLATE = lib
CONFIG += plugin

HEADERS = blur.h
SOURCES = blur.cpp
FORMS = blurwidget.ui
TARGET = blur
DESTDIR = .
INCLUDEPATH += ../../app
CONFIG += qt warn_on
CONFIG -= debug_and_release

