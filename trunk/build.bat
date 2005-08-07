@rem vim: set fileformat=dos:
@echo off

if "%1"=="" (
    echo debug or release should be stated in argument 1
    goto bottom
)

rem I want to find a better technique here! - WJC
qmake CONFIG+=%1
cd app
qmake CONFIG+=%1
cd ..\lua
qmake CONFIG+=%1
cd ..\plugins
qmake CONFIG+=%1
cd test
qmake CONFIG+=%1
cd ..\..
make

:bottom