TEMPLATE = lib
CONFIG += staticlib dll

DESTDIR = ..
HEADERS += PixmapView.h PixmapViewTool.h PixmapDividedZoomer.h
SOURCES += PixmapView.cpp PixmapViewTool.cpp PixmapDividedZoomer.cpp
TARGET = pixview

CONFIG += warn_on
CONFIG -= debug_and_release

