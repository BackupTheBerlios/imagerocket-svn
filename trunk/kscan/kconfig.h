/* KDialogBase
   This is a simplified replacement for KDE's KConfig for porting from KDE.
   Copyright (C) 2006 Wesley Crossman <wesley@crossmans.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef K_CONFIG
#define K_CONFIG

#include <QtCore>

namespace KConfigBase {
    enum Visibility {Normal, Global};
}

class KConfig : public QSettings {
Q_OBJECT
public:
    KConfig();
    KConfig(QString file, bool = false);
    void setGroup(QString group);
    QString readEntry(QString entry, QString value = QString::null);
    bool writeEntry(QString entry, QVariant value, int visibility = 0);
    bool writeEntry(QString entry, QString value, int visibility = 0);
    bool writeEntry(QString entry, bool value, int visibility = 0);
};

class KSimpleConfig : public KConfig {
public:
    KSimpleConfig(QString file, bool = false) : KConfig(file) {}
};

class KGlobal {
private:
    KGlobal();
    static KConfig *kcg;
public:
    static KConfig *config() {
        if (!kcg) kcg = new KConfig();
        return kcg;
    }
};

#endif
