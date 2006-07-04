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

#ifndef SCANNER_SELECT_DIALOG_H
#define SCANNER_SELECT_DIALOG_H


#include "ui_scannerselectdialog.h"
#include <QButtonGroup>
#include <QByteArray>
#include <QList>

/**
 *  A utitlity class that lets the user select a scan device.
 *
 *  This class provides functions to get the scan device to use for an 
 *  application with scan support.
 *
 */

class ScannerSelectDialog: public QDialog, private Ui::ScannerSelectDialog {
Q_OBJECT
public:
    /**
     *  constructs the dialog class
     *  @param QWidget *parent - the parent
     *  @param QStrList backends - a list of device names retrieved from the scan device
     *  @param QStrList scannerNames - a list of corresponding human readable sanner names.
     */
    ScannerSelectDialog( QWidget *parent, QList< QByteArray >&, const QStringList& );
    ~ScannerSelectDialog();

    /**
     *  returns the device the user selected.
     *  @return a CString containing the technical name of the selected device (taken from
     *          the backends-list from the constructor)
     */
    QByteArray getSelectedDevice( void ) const;

public slots:
    void setScanSources( const QList< QByteArray >&, const QStringList& );

protected:
   QList< QByteArray > devices;
   QButtonGroup buttonGroup;
};

#endif
