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

#include "kconfig.h"

KConfig *KGlobal::kcg = NULL;

KConfig::KConfig() {
}

KConfig::KConfig(QString confFile, bool) : QSettings(confFile, QSettings::IniFormat) {
}

void KConfig::setGroup(QString group) {
    beginGroup(group);
}

bool KConfig::writeEntry(QString entry, QVariant value, int visibility) {
    setValue(entry, value);
    return true;
}

bool KConfig::writeEntry(QString entry, QString value, int visibility) {
    setValue(entry, value);
    return true;
}

bool KConfig::writeEntry(QString entry, bool value, int visibility) {
    setValue(entry, value);
    return true;
}

QString KConfig::readEntry(QString entry, QString defaultValue) {
    return value(entry).toString();
}
