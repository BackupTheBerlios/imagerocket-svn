set(QT_USE_QTNETWORK true)
include(${QT_USE_FILE})
# the variable "qtproject_SRCS" contains all .cpp files of this project
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

qt4_wrap_ui(${imagerocket_SRCS} 
    aboutdialog.ui
    ftpbrowserdialog.ui
    ftpdialog.ui
    infotool.ui
    optionsdialog.ui
    savedialog.ui
    savesettingstool.ui
    updatedialog.ui
)
qt4_automoc(${imagerocket_SRCS})
add_executable(imagerocket ${imagerocket_SRCS})
target_link_libraries(imagerocket ${QT_LIBRARIES})

