project(imagerocket)
include(CPack)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

cmake_minimum_required(VERSION 2.4.0)

find_package(Qt4 REQUIRED)
if (UNIX)
    find_package(Sane)
endif (UNIX)

add_subdirectory(pixmapview)
if (SANE_FOUND)
    add_subdirectory(kscan)
endif (SANE_FOUND)
add_subdirectory(app)
add_subdirectory(plugins)

if (WIN32)
    set(ROOT ${QT_LIBRARY_DIR}/..)
    install(FILES ${ROOT}/bin/mingwm10.dll DESTINATION .)
    if (DEBUG)
        install(FILES ${ROOT}/bin/QtCored4.dll DESTINATION .)
        install(FILES ${ROOT}/bin/QtGuid4.dll DESTINATION .)
        install(FILES ${ROOT}/bin/QtNetworkd4.dll DESTINATION .)
    else (DEBUG)
        install(FILES ${ROOT}/bin/QtCore4.dll DESTINATION .)
        install(FILES ${ROOT}/bin/QtGui4.dll DESTINATION .)
        install(FILES ${ROOT}/bin/QtNetwork4.dll DESTINATION .)
    endif (DEBUG)
endif (WIN32)

