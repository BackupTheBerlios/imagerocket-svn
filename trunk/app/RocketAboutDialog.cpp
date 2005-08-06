#include "RocketAboutDialog.h"
#include "consts.h"
#include <QFile>
#include <QTextStream>

RocketAboutDialog::RocketAboutDialog(QWidget *parent)
        : QDialog(parent) {
    setupUi(this);
    setWindowTitle(tr("About %1").arg(PROGRAM_NAME));
    
    //Title
    QString tmp;
    tmp.append(tr("<h2>" PROGRAM_NAME " " VERSION "</h2>"
            "<h4>"
            "&copy; 2005 Wesley Crossman<br>"
            "Lua scripting engine &copy; 1994-2004 Tecgraf, PUC-Rio<br>"
            "Qt toolkit &copy; 1994-2005 Trolltech AS"
            "</h4></br>"));
    //Credits
    tmp.append(tr("<h3>Thanks To</h3><br><ul>"
            "<li></li>"
            "</ul>"));
    //License
    QFile f(":/license.txt");
    f.open(QFile::ReadOnly);
    QTextStream in(&f);
    QString license(in.readAll().replace("\n", "<br>"));
    tmp.append(tr("<h3>License</h3><br><tt><small>"
            "<i>This program is distributed under the GPL, version 2 or later.</i>"
            "<br>%1</small></tt>").arg(license));
    aboutEdit->setHtml(tmp);
    
    aboutEdit->setReadOnly(true);
}
