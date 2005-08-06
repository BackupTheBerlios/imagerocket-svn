TEMPLATE = app
TARGET = imgrocket
DESTDIR = ..
HEADERS =   RocketWindow.h \
            RocketAboutDialog.h \
            RocketView.h \
            RocketImageSquareContainer.h \
            RocketFilePreviewArea.h \
            RocketFilePreviewWidget.h \
            RocketImageList.h \
            RocketImage.h \
            RocketToolBox.h \
            ThreadedThumbnailGenerator.h \
            interfaces.h \
            consts.h
SOURCES =   main.cpp \
            RocketWindow.cpp \
            RocketAboutDialog.cpp \
            RocketView.cpp \
            RocketImageSquareContainer.cpp \
            RocketFilePreviewArea.cpp \
            RocketFilePreviewWidget.cpp \
            RocketImageList.cpp \
            RocketToolBox.cpp \
            RocketImage.cpp \
            ThreadedThumbnailGenerator.cpp
            
FORMS =     aboutdialog.ui
RESOURCES = pixmaps.qrc app.qrc
INCLUDEPATH += ../lua
MOC_DIR = tmp
OBJECTS_DIR = tmp
UI_DIR = tmp
RCC_DIR = tmp
CONFIG += warn_on
CONFIG -= debug_and_release
win32:LIBS += ../lua.dll
unix:LIBS += ../liblua.so

