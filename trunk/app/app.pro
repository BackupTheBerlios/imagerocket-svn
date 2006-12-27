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
            RocketSaveSettingsTool.h \
            RocketFtpDialog.h \
            RocketFtpBrowserDialog.h \
            ThreadedImageLoader.h \
            FtpBrowserModel.h \
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
            RocketSaveSettingsTool.cpp \
            RocketFtpDialog.cpp \
            RocketFtpBrowserDialog.cpp \
            ThreadedImageLoader.cpp \
            FtpBrowserModel.cpp

FORMS =     aboutdialog.ui \
            optionsdialog.ui \
            savedialog.ui \
            updatedialog.ui \
            infotool.ui \
            savesettingstool.ui \
            ftpdialog.ui \
            ftpbrowserdialog.ui
RESOURCES = pixmaps.qrc app.qrc
INCLUDEPATH += ../pixmapview .
unix {
    INCLUDEPATH += ../kscan
}
win32 {
    RC_FILE = windows.rc
}
MOC_DIR = tmp
OBJECTS_DIR = tmp
UI_DIR = tmp
RCC_DIR = tmp
CONFIG += warn_on precompile_header
CONFIG -= debug_and_release
QT += network
win32:LIBS += ../pixview.dll -lwininet
unix:LIBS += ../libpixview.so ../libkscan.so -lsane

