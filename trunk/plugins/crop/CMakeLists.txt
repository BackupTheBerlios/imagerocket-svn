#needed for importing generated UIs in out-of-source compiles
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(plugin_SRCS
    crop.cpp
    CropViewTool.cpp
)

set(plugin_MOC_HDRS
    crop.h
    CropViewTool.h
)

set(plugin_UIS
    cropwidget.ui
)

qt4_wrap_cpp(plugin_MOC_SRCS ${plugin_MOC_HDRS})
qt4_wrap_ui(plugin_UIS_H ${plugin_UIS})
add_library(plugin SHARED ${plugin_SRCS} ${plugin_MOC_SRCS} ${plugin_UIS_H})
target_link_libraries(plugin ${QT_LIBRARIES} pixmapview)
install(TARGETS plugin DESTINATION plugins/crop)
install(FILES crop.png DESTINATION plugins/crop)

