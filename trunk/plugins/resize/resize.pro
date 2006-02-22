TEMPLATE = lib
CONFIG += plugin

HEADERS = resize.h ResizeViewTool.h
SOURCES = resize.cpp ResizeViewTool.cpp
RESOURCES = resize.qrc
FORMS = resizewidget.ui
TARGET = resize
DESTDIR = .
INCLUDEPATH += ../../app ../../pixmapview
CONFIG += qt warn_on
CONFIG -= debug_and_release
win32:LIBS += ../../pixview.dll
unix:LIBS += ../../libpixview.so

