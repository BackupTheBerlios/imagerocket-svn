TEMPLATE = lib
CONFIG += plugin

HEADERS = gamma.h
SOURCES = gamma.cpp
FORMS = gammawidget.ui
TARGET = gamma
DESTDIR = .
INCLUDEPATH += ../../app
CONFIG += qt warn_on
CONFIG -= debug_and_release

