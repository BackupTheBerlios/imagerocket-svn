/* ImageRocket
An image-editing program written for editing speed and ease of use.
Copyright (C) 2005-2006 Wesley Crossman
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
#include "ProgramStarter.h"
#include "consts.h"
#include <QFile>
#include <QTextStream>

RocketAboutDialog::RocketAboutDialog(QWidget *parent)
        : QDialog(parent) {
    setupUi(this);
    connect(homepageButton, SIGNAL(clicked()), SLOT(homepageButtonClicked()));
    connect(homepageButton, SIGNAL(clicked()), SLOT(close()));
    setWindowTitle(tr("About %1").arg(PROGRAM_NAME));
    
    //Title
    QString tmp;
    tmp.append(tr("<h2>" PROGRAM_NAME " " VERSION "</h2>"
            "<h4>"
            "&copy; 2005-2006 Wesley Crossman<br>"
            "Qt toolkit &copy; 1994-2006 Trolltech AS<br>"
            "Icons are from Red Hat's Bluecurve and The Gimp's default icon theme"
            "</h4></br>"));
    //Credits
    tmp.append(tr("<h3>Thanks To</h3><br><ul>"
            "<li></li>"
            "</ul>"));
    //License
    QFile f(":/license.htm");
    f.open(QFile::ReadOnly);
    QTextStream in(&f);
    QString license(in.readAll());
    tmp.append(tr("<h3>License</h3><br><tt>"
            "<i>This program is distributed under the GPL, version 2 or later.</i>"
            "<br>%1</tt>").arg(license));
    aboutEdit->setHtml(tmp);
    
    aboutEdit->setReadOnly(true);
}

void RocketAboutDialog::homepageButtonClicked() {
    ProgramStarter::instance()->openWebBrowser(
            QString(HOMEPAGE) + tr("/", "language-specific homepage subdirectory"));
}
