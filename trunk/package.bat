@rem vim: set fileformat=dos:
@echo off

if not exist build (
    mkdir build
    copy %QTDIR%\bin\QtCore4.dll build
    copy %QTDIR%\bin\QtGui4.dll build
    mkdir build\imageformats
    rem  XXX: In the future, only the codecs for the current type of
    rem  build (debug or release) should be included in the zip, since
    rem  the debug codecs take up ~500 kb. - WJC
    copy %QTDIR%\plugins\imageformats\*.dll build\imageformats
    
    if exist c:\mingw\bin\mingwm10.dll (
        copy c:\mingw\bin\mingwm10.dll build
    ) else if exist e:\mingw\bin\mingwm10.dll (
        copy e:\mingw\bin\mingwm10.dll build
    ) else (
        echo Can't find MinGW support DLL
    )
) else (
    echo Using qt/mingw-support DLL's in "build" dir for zip file
    echo Delete "build" dir to re-copy.
)
copy imagerocket.exe build
mkdir build\plugins
xcopy /S plugins build\plugins
cd build
if exist imagerocket-exe.zip (del imagerocket-exe.zip)
..\zip a -tzip imagerocket-exe.zip *
cd ..
