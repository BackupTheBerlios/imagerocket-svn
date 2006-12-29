set(CMAKE_VERBOSE_MAKEFILE true)
set(QT_USE_QTNETWORK true)
set(QT_USE_QTMAIN true)
set(EXECUTABLE_OUTPUT_PATH ..)
include(${QT_USE_FILE})

include_directories(../pixmapview)
if (UNIX)
    include_directories(../kscan)
endif (UNIX)

set(imagerocket_SRCS
    FtpBrowserModel.cpp
    RocketAboutDialog.cpp
    RocketFilePreviewArea.cpp
    RocketFilePreviewWidget.cpp
    RocketFtpBrowserDialog.cpp
    RocketFtpDialog.cpp
    RocketImage.cpp
    RocketImageList.cpp
    RocketInfoTool.cpp
    RocketOptionsDialog.cpp
    RocketSaveDialog.cpp
    RocketSaveSettingsTool.cpp
    RocketToolBox.cpp
    RocketUpdateChecker.cpp
    RocketWindow.cpp
    ThreadedImageLoader.cpp
    main.cpp
)

set(imagerocket_MOC_HDRS
    RocketWindow.h
    RocketAboutDialog.h
    RocketOptionsDialog.h
    RocketUpdateChecker.h
    RocketSaveDialog.h
    RocketFilePreviewArea.h
    RocketFilePreviewWidget.h
    RocketImageList.h
    RocketImage.h
    RocketToolBox.h
    RocketInfoTool.h
    RocketSaveSettingsTool.h
    RocketFtpDialog.h
    RocketFtpBrowserDialog.h
    ThreadedImageLoader.h
    FtpBrowserModel.h
)

set(imagerocket_UIS
    aboutdialog.ui
    ftpbrowserdialog.ui
    ftpdialog.ui
    infotool.ui
    optionsdialog.ui
    savedialog.ui
    savesettingstool.ui
    updatedialog.ui
)
qt4_wrap_cpp(imagerocket_MOC_SRCS ${imagerocket_MOC_HDRS})
qt4_wrap_ui(imagerocket_UIS_H ${imagerocket_UIS})
qt4_automoc(${imagerocket_SRCS})
add_executable(imagerocket WIN32 ${imagerocket_SRCS} ${imagerocket_MOC_SRCS} ${imagerocket_UIS_H} )
target_link_libraries(imagerocket ${QT_LIBRARIES} pixmapview wininet)

