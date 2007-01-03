set(LIBRARY_OUTPUT_PATH ..)
include(${QT_USE_FILE})

set(pixmapview_SRCS
    PixmapDividedZoomer.cpp
    PixmapView.cpp
    PixmapViewTool.cpp
)

set(pixmapview_MOC_HDRS
    PixmapDividedZoomer.h
    PixmapView.h
)

qt4_wrap_cpp(pixmapview_MOC_SRCS ${pixmapview_MOC_HDRS})
qt4_automoc(${pixmapview_SRCS})
add_library(pixmapview SHARED ${pixmapview_SRCS} ${pixmapview_MOC_SRCS})
target_link_libraries(pixmapview ${QT_LIBRARIES})
if (UNIX)
    install(TARGETS pixmapview DESTINATION lib)
else (UNIX)
    install(TARGETS pixmapview DESTINATION .)
endif (UNIX)

