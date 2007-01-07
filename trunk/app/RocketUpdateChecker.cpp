/* ImageRocket
An image-editing program written for editing speed and ease of use.
Copyright (C) 2005 Wesley Crossman
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

#include "RocketUpdateChecker.h"
#include "config.h"

#ifdef Q_WS_WIN
#include <wininet.h>
#endif

RocketUpdateChecker::RocketUpdateChecker(QObject *parent, bool userRequestedCheck)
	    : QObject(parent) {
    returned = upgradable = error = false;
    this->userRequestedCheck = userRequestedCheck;
    connect(&http, SIGNAL(done(bool)), SLOT(httpDone(bool)));
    http.setHost("www.crossmans.net");
    http.get("/imagerocket-update.htm", &getBuffer);
}

RocketUpdateChecker::~RocketUpdateChecker() {
}

bool RocketUpdateChecker::mayBeConnected() {
    bool connected = true;
#ifdef Q_WS_WIN
    if (!InternetGetConnectedState(NULL, 0)) {
        connected = false;
    }
    qDebug() << connected;
#endif
    return connected;
}

RocketUpdateDialog *RocketUpdateChecker::createDialog(QWidget *parent) {
    RocketUpdateDialog *dialog = new RocketUpdateDialog(html);
    if (userRequestedCheck) {
        //makes no sense in this context
        dialog->dontTellMeButton->setVisible(false);
    }
    return dialog;
}

void RocketUpdateChecker::httpDone(bool error) {
    returned = true;
    this->error = error;
    if (!error) {
        QTextStream ts(&getBuffer.buffer(), QIODevice::ReadOnly);
        html = ts.readAll();
        qDebug(QString("version=").append(VERSION).toAscii());
        upgradable = html.indexOf(QString("version=").append(VERSION)) == -1;
    }
    emit done(error);
}

RocketUpdateDialog::RocketUpdateDialog(QString &html) {
    selected = 0;
    setupUi(this);
    connect(takeMeButton, SIGNAL(clicked()), SLOT(takeMeClicked()));
    connect(dontTellMeButton, SIGNAL(clicked()), SLOT(dontTellMeClicked()));
    updateEdit->setHtml(html);
}

void RocketUpdateDialog::takeMeClicked() {
    selected = 1;
    accept();
}

void RocketUpdateDialog::dontTellMeClicked() {
    selected = 2;
    accept();
}
