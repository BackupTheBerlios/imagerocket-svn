#include "RocketWindow.h"
#include <QApplication>
#include <QString>
#include <QStringList>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("Wesley Crossman");
    QCoreApplication::setApplicationName("ImageRocket");
    
    lua_State *L = lua_open();
    luaopen_base(L);
    luaopen_table(L);
    luaopen_io(L);
    luaopen_string(L);
    luaopen_math(L);
    luaopen_debug(L);
    luaopen_os(L);
    
    RocketWindow *win = new RocketWindow(L);
    if (app.argc() > 1) {
        win->setDirectory(app.argv()[1]);
    }
    win->show();
    return app.exec();
}
