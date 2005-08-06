rem vim: set fileformat=dos:

if not exist build (
    mkdir build
    copy lua.dll build
    copy %QTDIR%\bin\QtCore4.dll build
    copy %QTDIR%\bin\QtGui4.dll build
    rem should be one of the two
    copy c:\mingw\bin\mingwm10.dll build
    copy e:\mingw\bin\mingwm10.dll build
) else (
    echo Using existing lua/qt/mingw-support DLL's for zip file
    echo Delete build dir for recreation
)
copy imgrocket.exe build

