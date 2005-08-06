rem vim: set fileformat=dos:

rem I want to find a better technique here! - WJC
qmake CONFIG+=release
cd app
qmake CONFIG+=release
cd ..\lua
qmake CONFIG+=release
cd ..\plugins
qmake CONFIG+=release
cd test
qmake CONFIG+=release
cd ..\..
make

