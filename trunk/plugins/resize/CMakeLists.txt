set(plugin_SRCS
    resize.cpp
    ResizeViewTool.cpp
)

set(plugin_MOC_HDRS
    resize.h
    ResizeViewTool.h
)

set(plugin_UIS
    resizewidget.ui
)

set(plugin_RES
    resize.qrc
)

qt4_wrap_cpp(plugin_MOC_SRCS ${plugin_MOC_HDRS})
qt4_wrap_ui(plugin_UIS_H ${plugin_UIS})
add_library(plugin SHARED ${plugin_SRCS} ${plugin_MOC_SRCS} ${plugin_UIS_H} ${plugin_RES_SRCS})
target_link_libraries(plugin ${QT_LIBRARIES} pixmapview)

