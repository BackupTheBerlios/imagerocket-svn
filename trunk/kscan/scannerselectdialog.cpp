/* ScannerSelectDialog
   A simplified version of KScan's devselector.
   Copyright (C) 2006 Wesley Crossman <wesley@crossmans.net>
   Based on the file devselector.h in libkscan

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

#include "scannerselectdialog.h"
#include <QtGui>

ScannerSelectDialog::ScannerSelectDialog(QWidget *parent, QList< QByteArray >& devList,
				const QStringList& hrdevList) : QDialog(parent) {
    setupUi(this);
    setScanSources(devList, hrdevList);
}

QByteArray ScannerSelectDialog::getSelectedDevice( void ) const {
    int selID = -1;
    QList < QAbstractButton * > btns(buttonGroup.buttons());
    for (selID=0;selID<btns.size();++selID) if (btns[selID]->isChecked()) break;
    int dcount = devices.count();
    qDebug() << "The Selected ID is <" << selID << ">/" << dcount;
    if (selID < 0) return 0;
    const char * dev = devices.at( selID );
    qDebug() << "The selected device: <" << dev << ">";
    return dev;
}


void ScannerSelectDialog::setScanSources(const QList< QByteArray >& sources, const QStringList& hrSources) {
    for (int a=0;a<sources.size();++a) {
        QString text = QString::fromLatin1("&%1. %2")
                .arg(a+1).arg(hrSources[a]); //arg(QString::fromLocal8Bit(sources[a]));
        QRadioButton *rb = new QRadioButton(text, scannerBox);
        if (a == 0) rb->setChecked(true);
        scannerBox->layout()->addWidget(rb);
        buttonGroup.addButton(rb);
        devices.append(sources[a]);
    }
    
}

ScannerSelectDialog::~ScannerSelectDialog() {
}
