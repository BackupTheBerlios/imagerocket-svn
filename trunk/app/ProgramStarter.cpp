/* ProgramStarter
This is a class for starting programs, such as web browsers.
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

#include "ProgramStarter.h"

ProgramStarter *ProgramStarter::m_instance = 0;

ProgramStarter::ProgramStarter() {
    /*timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), SLOT(timeout()));
    timer.start(75);*/
}

ProgramStarter::~ProgramStarter() {
}

void ProgramStarter::openWebBrowser(QString address) {
    //XXX This function is broken until Qt bug #90175 is fixed (not visible to the public when
    //this was written). Briefly, startDetached seems to cause a deadlock if the DNS lookup
    //thread is running. Trolltech's response: "Indeed, we see this here as well. We'll fix
    //this for an upcoming release" - WJC
    /*QStringList webBrowserCommands;
    webBrowserCommands << "urlview" << "x-www-browser" << "firefox";
    webBrowserCommands << "mozilla-firefox" << "konqueror";
    InternalJob *job = new InternalJob;
    job->commands = webBrowserCommands;
    job->args = QStringList(address);
    foreach (QString a, webBrowserCommands) {
        
    }*/
    //QProcess::startDetached("firefox", QStringList(address));
}

/*void ProgramStarter::timeout() {
    foreach (InternalJob *a, jobs) {
        
    }
}*/
