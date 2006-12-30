set(plugin_SRCS
    bc.cpp
)

set(plugin_MOC_HDRS
    bc.h
)

set(plugin_UIS
    bcwidget.ui
)

qt4_wrap_cpp(plugin_MOC_SRCS ${plugin_MOC_HDRS})
qt4_wrap_ui(plugin_UIS_H ${plugin_UIS})
add_library(plugin SHARED ${plugin_SRCS} ${plugin_MOC_SRCS} ${plugin_UIS_H})
target_link_libraries(plugin ${QT_LIBRARIES})

