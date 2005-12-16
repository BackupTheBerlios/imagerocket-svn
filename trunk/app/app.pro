TEMPLATE = app
TARGET = imagerocket
DESTDIR = ..
HEADERS =   RocketWindow.h \
            RocketAboutDialog.h \
            RocketUpdateChecker.h \
            RocketSaveDialog.h \
            RocketFilePreviewArea.h \
            RocketFilePreviewWidget.h \
            RocketImageList.h \
            RocketImage.h \
            RocketToolBox.h \
            SaveSettingsTool.h \
            ProgramStarter.h \
            ThreadedImageLoader.h \
            interfaces.h \
            consts.h
SOURCES =   main.cpp \
            RocketWindow.cpp \
            RocketAboutDialog.cpp \
            RocketUpdateChecker.cpp \
            RocketSaveDialog.cpp \
            RocketFilePreviewArea.cpp \
            RocketFilePreviewWidget.cpp \
            RocketImageList.cpp \
            RocketToolBox.cpp \
            RocketImage.cpp \
            SaveSettingsTool.cpp \
            ProgramStarter.cpp \
            ThreadedImageLoader.cpp

FORMS =     aboutdialog.ui savedialog.ui updatedialog.ui savesettingstool.ui
RESOURCES = pixmaps.qrc app.qrc
INCLUDEPATH += ../lua ../pixmapview .
MOC_DIR = tmp
OBJECTS_DIR = tmp
UI_DIR = tmp
RCC_DIR = tmp
CONFIG += warn_on
CONFIG -= debug_and_release
QT += network
win32:LIBS += ../lua.dll ../pixview.dll
unix:LIBS += ../liblua.so ../libpixview.so

