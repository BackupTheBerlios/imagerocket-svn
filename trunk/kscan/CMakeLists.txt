#needed for importing generated UIs in out-of-source compiles
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(LIBRARY_OUTPUT_PATH ..)
set(QT_USE_QT3SUPPORT true)
include(${QT_USE_FILE})
include(${SANE_INCLUDE_DIR})

set(kscan_SRCS
    imgscaninfo.cpp
    kgammatable.cpp
    kscandevice.cpp
    kscanoption.cpp
    kscanoptset.cpp
    kscanslider.cpp
    kconfig.cpp
    lightpreviewwidget.cpp
    scannerselectdialog.cpp
    scannerdialog.cpp
)

set(kscan_MOC_HDRS
    kgammatable.h
    kscandevice.h
    kscanoption.h
    kscanslider.h
    kconfig.h
    lightpreviewwidget.h
    scannerselectdialog.h
    scannerdialog.h
)

set(kscan_UIS
    scannerselectdialog.ui
)

qt4_wrap_ui(kscan_UIS_H ${kscan_UIS})
qt4_wrap_cpp(kscan_MOC_SRCS ${kscan_MOC_HDRS})
qt4_automoc(${kscan_SRCS})
add_library(kscan SHARED ${kscan_SRCS} ${kscan_MOC_SRCS} ${kscan_UIS})
target_link_libraries(kscan ${QT_LIBRARIES} ${SANE_LIBRARY})

