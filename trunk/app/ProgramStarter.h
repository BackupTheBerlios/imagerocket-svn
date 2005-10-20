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

#ifndef PROGRAM_STARTER
#define PROGRAM_STARTER

#include <QtCore>

class ProgramStarter : public QObject {
Q_OBJECT
public:
    ~ProgramStarter();
    static ProgramStarter *instance();
    void openWebBrowser(QString address);
protected:
    ProgramStarter();
private:
    static ProgramStarter *m_instance;
};

inline ProgramStarter *ProgramStarter::instance() {
    if (!m_instance) {
        m_instance = new ProgramStarter;
    }
    return m_instance;
}

#endif
