project(imagerocket) # the name of your project

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

