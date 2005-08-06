TEMPLATE = lib
CONFIG += plugin

HEADERS = test.h
SOURCES = test.cpp
TARGET = test
DESTDIR = .
INCLUDEPATH += ../../app ../../lua
CONFIG += qt warn_on
CONFIG -= debug_and_release
win32:LIBS += ../../lua.dll
unix:LIBS += ../../liblua.so

