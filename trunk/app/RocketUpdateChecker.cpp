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
#include "consts.h"

#ifdef false //Q_WS_WIN
BOOL InternetGetConnectedState(
    LPDWORD lpdwFlags,
    DWORD dwReserved
);
#define INTERNET_CONNECTION_CONFIGURED 0x40
#define INTERNET_CONNECTION_LAN 0x02
#define INTERNET_CONNECTION_MODEM 0x01
#define INTERNET_CONNECTION_OFFLINE 0x20
#define INTERNET_CONNECTION_PROXY 0x04
#define INTERNET_RAS_INSTALLED 0x10
#endif

RocketUpdateChecker::RocketUpdateChecker(QObject *parent) : QObject(parent) {
    returned = upgradable = error = false;
    connect(&http, SIGNAL(done(bool)), SLOT(httpDone(bool)));
    http.setHost("www.crossmans.net");
    http.get("/imagerocket-update.htm", &getBuffer);
}

RocketUpdateChecker::~RocketUpdateChecker() {
}

bool RocketUpdateChecker::mayBeConnected() {
    bool connected = true;
#ifdef false //Q_WS_WIN
    if (!InternetGetConnectionState(INTERNET_CONNECTION_LAN, 0)
                && !InternetGetConnectionState(INTERNET_CONNECTION_MODEM, 0)) {
        connected = false;
    }
#endif
    return connected;
}

RocketUpdateDialog *RocketUpdateChecker::createDialog(QWidget *parent) {
    return new RocketUpdateDialog(html);
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
