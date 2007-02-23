/* ImageRocket
An image-editing program written for editing speed and ease of use.
Copyright (C) 2005-2007 Wesley Crossman
Email: wesley@crossmans.net

You can redistribute and/or modify this software under the terms of the GNU
General Public License as published by the Free Software Foundation;
either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this
program; if not, write to the Free Software Foundation, Inc., 59 Temple Place,
Suite 330, Boston, MA 02111-1307 USA */

#include "RocketAboutDialog.h"
#include "config.h"
#include <QFile>
#include <QTextStream>
#include <QDesktopServices>
#include <QUrl>

RocketAboutDialog::RocketAboutDialog(QWidget *parent)
        : QDialog(parent) {
    setupUi(this);
    connect(homepageButton, SIGNAL(clicked()), SLOT(homepageButtonClicked()));
    connect(homepageButton, SIGNAL(clicked()), SLOT(close()));
    setWindowTitle(tr("About %1").arg(PROGRAM_NAME));
    
    //Title
    QString tmp;
    tmp += "<h2>";
    tmp += tr("%1 %2", "about dialog header").arg(PROGRAM_NAME).arg(VERSION);
    tmp += "</h2><h4>";
#ifndef NDEBUG
    tmp += tr("Debug Mode", "about dialog message");
    tmp += "<br><br>";
#endif
    tmp += tr("&copy; 2005-2007 Wesley Crossman", "about dialog");
    tmp += "<br>";
    tmp += tr("Qt %1 &copy; Trolltech AS", "about dialog").arg(QT_VERSION_STR);
    tmp += "<br>";
    tmp += tr("Icons are from Red Hat's Bluecurve and The Gimp's default icon theme",
            "about dialog");
    tmp += "</h4></br>";
    //tmp += "<h3>";
    //tmp += tr("Thanks To");
    //tmp += "</h3><br><ul>";
    //Credits - Your name here!
    //tmp += "<li></li>";
    //tmp += "</ul>";
    //License
    QFile f(":/license.htm");
    f.open(QFile::ReadOnly);
    QTextStream in(&f);
    QString license(in.readAll());
    tmp += "<br><h3>";
    tmp += tr("License", "about dialog");
    tmp += "</h3><br><br><tt>";
    tmp += tr("<i>This program is distributed under the GPL, version 2 or later.</i>", "about dialog");
    tmp += "<br>";
    tmp += license;
    tmp += "</tt>";
    aboutEdit->setHtml(tmp);
    
    aboutEdit->setReadOnly(true);
}

void RocketAboutDialog::homepageButtonClicked() {
    QDesktopServices::openUrl(
            QUrl(QString(HOMEPAGE) + tr("/", "language-specific homepage subdirectory")));
}
