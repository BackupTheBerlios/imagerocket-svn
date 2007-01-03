include(${QT_USE_FILE})

include_directories(../app)
include_directories(../pixmapview)
add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DQT_NO_DEBUG)
else (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DQT_DEBUG)
endif (CMAKE_BUILD_TYPE STREQUAL "Release")

add_subdirectory(blur)
add_subdirectory(sharpen)
add_subdirectory(brightness_contrast)
add_subdirectory(gamma)
add_subdirectory(crop)
add_subdirectory(resize)

