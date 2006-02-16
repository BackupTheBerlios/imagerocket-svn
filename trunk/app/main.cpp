#include "RocketWindow.h"
#include <QApplication>
#include <QString>
#include <QStringList>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("Wesley Crossman");
    QCoreApplication::setApplicationName("ImageRocket");
    
    RocketWindow *win = new RocketWindow();
    if (app.argc() > 1) {
        win->setDirectory(app.argv()[1]);
    }
    win->show();
    return app.exec();
}
