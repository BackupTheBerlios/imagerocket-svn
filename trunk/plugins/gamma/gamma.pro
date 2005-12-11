TEMPLATE = lib
CONFIG += plugin

HEADERS = gamma.h
SOURCES = gamma.cpp
FORMS = gammawidget.ui
TARGET = gamma
DESTDIR = .
INCLUDEPATH += ../../app ../../lua
CONFIG += qt warn_on
CONFIG -= debug_and_release
win32:LIBS += ../../lua.dll
unix:LIBS += ../../liblua.so

