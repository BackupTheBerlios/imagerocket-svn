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

set(DLLS_TO_INSTALL
    "mingwm10.dll"
    "QtCore4.dll"
    "QtGui4.dll"
    "QtNetwork4.dll"
)
set(ROOT ${QT_LIBRARY_DIR}/..)
message("root: ${ROOT}")
foreach (A ${DLLS_TO_INSTALL})
    message(${A})
    find_file(LOCATION ${A} ${ROOT})
    if (NOTFOUND)
        message("Failed!")
    endif (NOTFOUND)
    message(${LOCATION})
    install(FILES ${LOCATION} DESTINATION .)
endforeach (A)


