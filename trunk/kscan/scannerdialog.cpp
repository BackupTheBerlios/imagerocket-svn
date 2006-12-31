/* ScannerDialog
   A simple qt-based scanning dialog
   Copyright (C) 2006 Wesley Crossman
   Portions of code from libkscan's scanparams.cpp
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

#undef ALWAYS_SHOW_SCANNER_SELECT_DIALOG

ScannerDialog::ScannerDialog(QWidget *parent) : QDialog(parent) {
    scannerConnected = true;
    setAttribute(Qt::WA_DeleteOnClose);
    resize(640, 480);
    layout = new QGridLayout(this);
    previewPicture = new LightPreviewWidget(this);
    connect(previewPicture, SIGNAL(selectionChanged(QRect)), SLOT(setScanRect(QRect)));
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
    device = new KScanDevice( this );
    connect(device, SIGNAL(sigNewImage(QImage *, ImgScanInfo *)),
            SLOT(imageReceived(QImage *, ImgScanInfo *)));
    connect(device, SIGNAL(sigNewPreview(QImage *, ImgScanInfo *)),
            SLOT(previewReceived(QImage *, ImgScanInfo *)));
    connect(device, SIGNAL(sigScanProgress(int)), prgStatus, SLOT(setValue(int)));
    QStringList scannerNames;
    QList< QByteArray > backends = device->getDevices();
    foreach (QByteArray ba, backends) {
        scannerNames.append( device->getScannerName( ba ));
    }

    /* ..if there are devices.. */
    QByteArray configDevice;
#ifdef ALWAYS_SHOW_SCANNER_SELECT_DIALOG
    if (scannerNames.count() > 0) {
#else
    if (scannerNames.count() == 1) {
        configDevice = backends[0];
    } else if (scannerNames.count() > 1) {
#endif
        /* allow the user to select one */
        ScannerSelectDialog ss(this, backends, scannerNames);
        if (ss.exec() == QDialog::Accepted) {
            configDevice = ss.getSelectedDevice();
        } else {
            scannerConnected = false;
        }
    }
    /* If a device was selected, */
    if (!configDevice.isNull() && scannerConnected) {
        device->openDevice(configDevice);
        if (!connectDevice(device)) {
            QMessageBox::warning(this, tr("Scanner"), tr("The scanner failed to start."));
            scannerConnected = false;
        }
    } else if (scannerConnected) {
        QMessageBox::warning(this, tr("Scanner"), tr("No scanners found."));
        scannerConnected = false;
    }
    if (!scannerConnected) {
        return;
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
    
    setScanRect(QRect(0, 0, 1000, 1000));
    
    //get dimensions to create dummy scanner bed for preview widget
    KScanOption tl_x(SANE_NAME_SCAN_TL_X);
    KScanOption tl_y(SANE_NAME_SCAN_TL_Y);
    KScanOption br_x(SANE_NAME_SCAN_BR_X);
    KScanOption br_y(SANE_NAME_SCAN_BR_Y);
    QRectF dimensions;
    double min1=0.0, max1=0.0, min2=0.0, max2=0.0, dummy1=0.0, dummy2=0.0;
    tl_x.getRange(&min1, &max1, &dummy1);
    tl_y.getRange(&min2, &max2, &dummy2);
    dimensions.setTopLeft(QPointF(min1, min2));
    br_x.getRange(&min1, &max1, &dummy1);
    br_y.getRange(&min2, &max2, &dummy2);
    dimensions.setBottomRight(QPointF(max1, max2));
    QSizeF size = dimensions.size() * 0.0393700787 * 75.0;
    QImage img(int(size.width()), int(size.height()), QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::white);
    previewPicture->setPixmap(QPixmap::fromImage(img));
}

ScannerDialog::~ScannerDialog() {
    delete device;
}

bool ScannerDialog::connectDevice(KScanDevice *device) {
    QList< QByteArray > strl = device->getCommonOptions();
    QScrollArea *area = new QScrollArea(this);
    area->setWidget(new QWidget(area));
    QBoxLayout *areaLayout = new QVBoxLayout(area->widget());
    QRegExp re("preview|.*-table|tl-x|tl-y|br-x|br-y");
    foreach (QByteArray ba, strl) {
        qDebug() << "Common: " << ba;
        if (!re.exactMatch(ba)) {
            KScanOption *option = device->getGuiElement(ba, area->widget());
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
    setRealScanRect(oldRect);
    previewPicture->setPixmap(QPixmap::fromImage(*img));
    prgStatus->reset();
}

void ScannerDialog::scanClicked() {
    device->acquire();
}

void ScannerDialog::previewClicked() {
    oldRect = getRealScanRect();
    setScanRect(QRect(0, 0, 1000, 1000));
    device->acquirePreview(false, 75);
}

void ScannerDialog::setScanRect(QRect sel) {
    //Derived from libkscan's scanparams.cpp
    qDebug() << "Custom-Size: " << sel.x() << ", " << sel.y() << " - " << sel.width() << "x" << sel.height();

    KScanOption tl_x( SANE_NAME_SCAN_TL_X );
    KScanOption tl_y( SANE_NAME_SCAN_TL_Y );
    KScanOption br_x( SANE_NAME_SCAN_BR_X );
    KScanOption br_y( SANE_NAME_SCAN_BR_Y );

    double min1=0.0, max1=0.0, min2=0.0, max2=0.0, dummy1=0.0, dummy2=0.0;
    
    tl_x.getRange( &min1, &max1, &dummy1 );
    br_x.getRange( &min2, &max2, &dummy2 );
    double range = max2-min1;
    double w = min1 + double(range * (double(sel.x()) / 1000.0) );
    tl_x.set( w );
    w = min1 + double(range * double(sel.x() + sel.width())/1000.0);
    br_x.set( w );
    qDebug() << "set tl_x: " << min1 + double(range * (double(sel.x()) / 1000.0) );
    qDebug() << "set br_x: " << min1 + double(range * (double(sel.x() + sel.width())/1000.0));

    tl_y.getRange( &min1, &max1, &dummy1 );
    br_y.getRange(&min2, &max2, &dummy2 );
    range = max2-min1;
    w = min1 + range * double(sel.y()) / 1000.0;
    tl_y.set( w );
    w = min1 + range * double(sel.y() + sel.height())/1000.0;
    br_y.set( w );
    qDebug() << "set tl_y: " << min1 + double(range * (double(sel.y()) / 1000.0) );
    qDebug() << "set br_y: " << min1 + double(range * (double(sel.y() + sel.height())/1000.0));

    device->apply( &tl_x );
    device->apply( &tl_y );
    device->apply( &br_x );
    device->apply( &br_y );
}

QRect ScannerDialog::getRealScanRect() {
    int x=0, y=0, x2=0, y2=0;
    KScanOption tl_x( SANE_NAME_SCAN_TL_X ), tl_y( SANE_NAME_SCAN_TL_Y );
    KScanOption br_x( SANE_NAME_SCAN_BR_X ), br_y( SANE_NAME_SCAN_BR_Y );
    assert(tl_x.get(&x) && tl_y.get(&y) && br_x.get(&x2) && br_y.get(&y2));
    return QRect(x, y, x2-x, y2-y);
}

void ScannerDialog::setRealScanRect(QRect sel) {
    //Derived from libkscan's scanparams.cpp
    qDebug() << "Custom-Size: " << sel.x() << ", " << sel.y() << " - " << sel.width() << "x" << sel.height();

    KScanOption tl_x( SANE_NAME_SCAN_TL_X ), tl_y( SANE_NAME_SCAN_TL_Y );
    KScanOption br_x( SANE_NAME_SCAN_BR_X ), br_y( SANE_NAME_SCAN_BR_Y );

    double min1=0.0, max1=0.0, min2=0.0, max2=0.0, dummy1=0.0, dummy2=0.0;
    
    tl_x.getRange( &min1, &max1, &dummy1 );
    br_x.getRange( &min2, &max2, &dummy2 );
    tl_x.set( sel.topLeft().x() );
    br_x.set( sel.bottomRight().x() );

    tl_y.getRange( &min1, &max1, &dummy1 );
    br_y.getRange( &min2, &max2, &dummy2 );
    tl_y.set( sel.topLeft().y() );
    br_y.set( sel.bottomRight().y() );

    device->apply( &tl_x );
    device->apply( &tl_y );
    device->apply( &br_x );
    device->apply( &br_y );
}
