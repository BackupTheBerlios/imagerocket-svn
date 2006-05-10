TEMPLATE = app
TARGET = imagerocket
DESTDIR = ..
PRECOMPILED_HEADER = precompiled.h
HEADERS =   RocketWindow.h \
            RocketAboutDialog.h \
            RocketOptionsDialog.h \
            RocketUpdateChecker.h \
            RocketSaveDialog.h \
            RocketFilePreviewArea.h \
            RocketFilePreviewWidget.h \
            RocketImageList.h \
            RocketImage.h \
            RocketToolBox.h \
            RocketInfoTool.h \
            SaveSettingsTool.h \
            ProgramStarter.h \
            ThreadedImageLoader.h \
            interfaces.h \
            consts.h \
            ImageTools.h
SOURCES =   main.cpp \
            RocketWindow.cpp \
            RocketAboutDialog.cpp \
            RocketOptionsDialog.cpp \
            RocketUpdateChecker.cpp \
            RocketSaveDialog.cpp \
            RocketFilePreviewArea.cpp \
            RocketFilePreviewWidget.cpp \
            RocketImageList.cpp \
            RocketToolBox.cpp \
            RocketImage.cpp \
            RocketInfoTool.cpp \
            SaveSettingsTool.cpp \
            ProgramStarter.cpp \
            ThreadedImageLoader.cpp

FORMS =     aboutdialog.ui optionsdialog.ui savedialog.ui updatedialog.ui infotool.ui savesettingstool.ui
RESOURCES = pixmaps.qrc app.qrc
INCLUDEPATH += ../lua ../pixmapview .
MOC_DIR = tmp
OBJECTS_DIR = tmp
UI_DIR = tmp
RCC_DIR = tmp
CONFIG += warn_on precompile_header
CONFIG -= debug_and_release
QT += network
win32:LIBS += ../pixview.dll
unix:LIBS += ../libpixview.so

