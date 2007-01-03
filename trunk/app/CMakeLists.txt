#needed for importing generated UIs in out-of-source compiles
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(QT_USE_QTNETWORK true)
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
if(MINGW)
    # resource compilation for mingw
    ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/windows.o
    COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_SOURCE_DIR}/windows.rc -o ${CMAKE_CURRENT_BINARY_DIR}/windows.o)
    SET(imagerocket_SRCS ${imagerocket_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/windows.o)
else(MINGW)
    if (WIN32)
        SET(imagerocket_SRCS ${imagerocket_SRCS} windows.rc)
    endif (WIN32)
endif(MINGW)


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

set(imagerocket_RES
    app.qrc
    pixmaps.qrc
)

qt4_wrap_cpp(imagerocket_MOC_SRCS ${imagerocket_MOC_HDRS})
qt4_wrap_ui(imagerocket_UIS_H ${imagerocket_UIS})
qt4_add_resources(imagerocket_RES_SRCS ${imagerocket_RES})
if (SANE_FOUND)
    add_definitions(-DSANE)
endif (SANE_FOUND)
add_executable(imagerocket WIN32 ${imagerocket_SRCS} ${imagerocket_MOC_SRCS} ${imagerocket_UIS_H} ${imagerocket_RES_SRCS})
target_link_libraries(imagerocket ${QT_LIBRARIES} pixmapview)
if (WIN32)
    target_link_libraries(imagerocket wininet)
endif (WIN32)
if (SANE_FOUND)
    target_link_libraries(imagerocket kscan)
endif (SANE_FOUND)
if (UNIX)
    install(TARGETS imagerocket DESTINATION bin)
else (UNIX)
    install(TARGETS imagerocket DESTINATION .)
endif (UNIX)

