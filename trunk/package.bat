@rem vim: set fileformat=dos:
@echo off

if not exist build (
    mkdir build
    copy lua.dll build
    copy %QTDIR%\bin\QtCore4.dll build
    copy %QTDIR%\bin\QtGui4.dll build
    mkdir build\imageformats
    copy %QTDIR%\plugins\imageformats\qjpeg*.dll build\imageformats
    
    if exist c:\mingw\bin\mingwm10.dll (
        copy c:\mingw\bin\mingwm10.dll build
    ) else if exist e:\mingw\bin\mingwm10.dll (
        copy e:\mingw\bin\mingwm10.dll build
    ) else (
        echo Can't find MinGW support DLL
    )
) else (
    echo Using lua/qt/mingw-support DLL's in "build" dir for zip file
    echo Delete "build" dir to re-copy.
)
copy imagerocket.exe build
cd build
if exist imagerocket-exe.zip (del imagerocket-exe.zip)
..\zip a -tzip imagerocket-exe.zip *
ftp -s:..\ftp_commands.txt crossmans.net
cd ..
