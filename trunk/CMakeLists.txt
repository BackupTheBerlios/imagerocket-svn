project(imagerocket) # the name of your project

cmake_minimum_required(VERSION 2.4.0)

find_package(Qt4 REQUIRED)

add_subdirectory(pixmapview)
if (UNIX)
    add_subdirectory(kscan)
endif (UNIX)
add_subdirectory(app)
add_subdirectory(plugins)

