set(LIBRARY_OUTPUT_PATH ..)
include(${QT_USE_FILE})
include_directories(.)
include_directories(private)

set(qtconcurrent_SRCS
    future.cpp
    futurewatcher.cpp
    map.cpp
    run.cpp
    private/threadpool.cpp
    private/threadmanager.cpp
    private/taskmanager.cpp
    private/futureengine.cpp
    private/futureresult.cpp
)

set(qtconcurrent_MOC_HDRS
    futurewatcher.h
    private/taskmanager.h
)

qt4_wrap_cpp(qtconcurrent_MOC_SRCS ${qtconcurrent_MOC_HDRS})
qt4_automoc(${qtconcurrent_SRCS})
add_library(qtconcurrent SHARED ${qtconcurrent_SRCS} ${qtconcurrent_MOC_SRCS})
target_link_libraries(qtconcurrent ${QT_LIBRARIES})
if (UNIX)
    install(TARGETS qtconcurrent DESTINATION lib)
else (UNIX)
    install(TARGETS qtconcurrent DESTINATION .)
endif (UNIX)

