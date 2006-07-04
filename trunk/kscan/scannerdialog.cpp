/* ScannerDialog
   A simple qt-based scanning dialog
   Copyright (C) 2006 Wesley Crossman
   Email: wesley@crossmans.net

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

#include "scannerdialog.h"
#include "lightpreviewwidget.h"
#include "scannerselectdialog.h"
#include "kscandevice.h"
#include "kscanoption.h"

ScannerDialog::ScannerDialog(QWidget *parent) : QDialog(parent) {
    setAttribute(Qt::WA_DeleteOnClose);
    resize(640, 480);
    layout = new QGridLayout(this);
    previewPicture = new LightPreviewWidget(this);
    btnScan = new QPushButton(tr("&Scan"), this);
    connect(btnScan, SIGNAL(clicked()), SLOT(scanClicked()));
    btnClose = new QPushButton(tr("&Close"), this);
    connect(btnClose, SIGNAL(clicked()), SLOT(reject()));
    btnPreview = new QPushButton(tr("&Preview"), this);
    connect(btnPreview, SIGNAL(clicked()), SLOT(previewClicked()));
    btnSelectAll = new QPushButton(tr("Select &All"), this);
    connect(btnSelectAll, SIGNAL(clicked()), previewPicture, SLOT(selectAll()));
    prgStatus = new QProgressBar(this);
    prgStatus->setRange(0, 100);
    /* continue to attach a real scanner */
    /* first, get the list of available devices from libkscan */
    m_device = new KScanDevice( this );
    connect(m_device, SIGNAL(sigNewImage(QImage *, ImgScanInfo *)),
            SLOT(imageReceived(QImage *, ImgScanInfo *)));
    connect(m_device, SIGNAL(sigNewPreview(QImage *, ImgScanInfo *)),
            SLOT(previewReceived(QImage *, ImgScanInfo *)));
    connect(m_device, SIGNAL(sigScanProgress(int)), prgStatus, SLOT(setValue(int)));
    QStringList scannerNames;
    QList< QByteArray > backends = m_device->getDevices();
    foreach (QByteArray ba, backends) {
        scannerNames.append( m_device->getScannerName( ba ));
    }

    /* ..if there are devices.. */
    QByteArray configDevice;
    bool good_scan_connect = true;
    if( scannerNames.count() > 0 )
    {
        /* allow the user to select one */
        ScannerSelectDialog ss( this, backends, scannerNames );

        if ( ss.exec() == QDialog::Accepted ) {
            configDevice = ss.getSelectedDevice();
        }

        /* If a device was selected, */
        if( ! configDevice.isNull() ) {
            /* ..open it (init sane with that) */
            m_device->openDevice( configDevice );

            /* ..and connect to the gui (create the gui) */
            if ( !connectDevice( m_device ) )
            {
                qDebug() << "ERR: Could not connect scan device";
                good_scan_connect = false;
            }
        }
    }
    layout->addWidget(btnScan, 1, 0);
    layout->addWidget(btnClose, 1, 1);
    QFrame *vline = new QFrame(this);
    vline->setFrameShape(QFrame::VLine);
    layout->addWidget(vline, 0, 2, 2, 1);
    layout->addWidget(previewPicture, 0, 3, 1, 2);
    layout->addWidget(btnPreview, 1, 3);
    layout->addWidget(btnSelectAll, 1, 4);
    layout->addWidget(prgStatus, 2, 0, 1, 5);
}

ScannerDialog::~ScannerDialog() {
    delete m_device;
}

bool ScannerDialog::connectDevice(KScanDevice *device) {
    sane_device = device;
    QList< QByteArray > strl = sane_device->getCommonOptions();
    QScrollArea *area = new QScrollArea(this);
    area->setWidget(new QWidget(area));
    QBoxLayout *areaLayout = new QVBoxLayout(area->widget());
    foreach ( QByteArray ba, strl ) {
        qDebug() << "Common: " << ba;
        if (ba == "preview" || !QString(ba).endsWith("-table")) {
            KScanOption *option = sane_device->getGuiElement(ba, area->widget());
            areaLayout->addWidget(option->widget());
        }
    }
    area->widget()->resize(areaLayout->sizeHint());
    //XXX I should be adding the width of the scrollbar, not an arbitrary value
    area->setMinimumWidth(areaLayout->sizeHint().width() + 40);
    layout->addWidget(area, 0, 0, 1, 2);
    
    return true;
}

void ScannerDialog::imageReceived(QImage *img, ImgScanInfo *info) {
    qDebug() << "Dialog: Image Received";
    emit imageScanned(QPixmap::fromImage(*img));
    prgStatus->reset();
}

void ScannerDialog::previewReceived(QImage *img, ImgScanInfo *info) {
    qDebug() << "Dialog: Preview Received";
    previewPicture->setPixmap(QPixmap::fromImage(*img));
    prgStatus->reset();
}

void ScannerDialog::scanClicked() {
    m_device->acquire();
}

void ScannerDialog::previewClicked() {
    m_device->acquirePreview(false, 75);
}
