project(imagerocket)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

cmake_minimum_required(VERSION 2.4.0)

find_package(Qt4 REQUIRED)
if (UNIX)
    find_package(Sane)
endif (UNIX)

set(CMAKE_PROJECT_NAME "ImageRocket")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "ImageRocket")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "5")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_VENDOR "Wesley Crossman")
#set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/readme.txt")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/license.txt")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CMAKE_PROJECT_NAME}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CMAKE_PROJECT_NAME}")
#set(CPACK_PACKAGE_EXECUTABLES "imagerocket;ImageRocket")

if (UNIX)
    if (APPLE)
        set(CPACK_GENERATOR "PackageMaker;STGZ;TGZ")
    else (APPLE)
        set(CPACK_GENERATOR "STGZ;TGZ;TBZ2")
    endif (APPLE)
    set(CPACK_SOURCE_GENERATOR "TGZ;TBZ2")
else (UNIX)
    set(CPACK_NSIS_URL_INFO_ABOUT "http://developer.berlios.de/projects/imagerocket")
    set(CPACK_NSIS_CREATE_ICONS "
            CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\ImageRocket.lnk\\\" \\\"$INSTDIR\\\\imagerocket.exe\\\"
	    ")
    set(CPACK_NSIS_DELETE_ICONS "
            Delete \\\"$SMPROGRAMS\\\\$MUI_TEMP\\\\ImageRocket.lnk\\\"
            ")
    set(CPACK_NSIS_MODIFY_PATH ON)
    set(CPACK_GENERATOR "NSIS;ZIP")
    set(CPACK_SOURCE_GENERATOR "ZIP")
endif (UNIX)


add_subdirectory(pixmapview)
if (SANE_FOUND)
    add_subdirectory(kscan)
endif (SANE_FOUND)
add_subdirectory(app)
add_subdirectory(plugins)

if (WIN32)
    set(ROOT ${QT_LIBRARY_DIR}/..)
    install(FILES ${ROOT}/bin/mingwm10.dll DESTINATION .)
    if (DEBUG)
        install(FILES ${ROOT}/bin/QtCored4.dll DESTINATION .)
        install(FILES ${ROOT}/bin/QtGuid4.dll DESTINATION .)
        install(FILES ${ROOT}/bin/QtNetworkd4.dll DESTINATION .)
    else (DEBUG)
        install(FILES ${ROOT}/bin/QtCore4.dll DESTINATION .)
        install(FILES ${ROOT}/bin/QtGui4.dll DESTINATION .)
        install(FILES ${ROOT}/bin/QtNetwork4.dll DESTINATION .)
    endif (DEBUG)
endif (WIN32)
install(FILES license.txt DESTINATION .)
if (UNIX)
    install(PROGRAMS imagerocket.sh DESTINATION bin)
endif (UNIX)

INCLUDE(InstallRequiredSystemLibraries)
include(CPack)

