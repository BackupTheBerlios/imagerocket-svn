TEMPLATE = lib
CONFIG += plugin

HEADERS = crop.h CropViewTool.h
SOURCES = crop.cpp CropViewTool.cpp
FORMS = cropwidget.ui
TARGET = crop
DESTDIR = .
INCLUDEPATH += ../../app ../../pixmapview ../../lua
CONFIG += qt warn_on
CONFIG -= debug_and_release
win32:LIBS += ../../lua.dll
unix:LIBS += ../../liblua.so

