/* ProgramStarter
This is a class for starting programs, such as web browsers.
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

#include "ProgramStarter.h"
#include <QMessageBox>

ProgramStarter *ProgramStarter::m_instance = 0;

ProgramStarter::ProgramStarter() {
}

ProgramStarter::~ProgramStarter() {
}

#ifdef Q_WS_WIN
#include <windows.h>
#endif

void ProgramStarter::openWebBrowser(QString address) {
    /*QStringList webBrowserCommands;
    webBrowserCommands << "urlview" << "x-www-browser" << "firefox";
    webBrowserCommands << "mozilla-firefox" << "konqueror";
    foreach (QString a, webBrowserCommands) {
        if (QProcess::startDetached(a, QStringList(address))) {
            break;
        }
    }*/
    //XXX support mac
    int ok = false;
#if defined(Q_WS_WIN)
    ok = int(ShellExecuteW(NULL, 0, (const WCHAR *)address.utf16(), 0, 0, SW_SHOWNORMAL)) > 32;
#elif defined(Q_WS_X11)
    ok = true;
    QProcess::startDetached("x-www-browser", QStringList(address));
#endif
    if (!ok) {
        QMessageBox::warning(NULL,
                tr("Error Opening Browser"),
                tr("The browser failed to open ") + address);
    }
}


/*void ProgramStarter::timeout() {
    foreach (InternalJob *a, jobs) {
        
    }
}*/
