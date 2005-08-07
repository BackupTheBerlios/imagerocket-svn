@rem vim: set fileformat=dos:
@echo off

if "%1"=="" (
    echo debug or release should be stated in argument 1
    goto bottom
)

make clean
call build.bat %1

:bottom
