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

#include <QtGui>
class KScanDevice;
class LightPreviewWidget;
class ImgScanInfo;

class ScannerDialog : public QDialog {
Q_OBJECT
public:
    ScannerDialog(QWidget *parent = NULL);
    ~ScannerDialog();
    bool connectDevice(KScanDevice *);
    bool isValid() {return true;}
signals:
    void imageScanned(const QPixmap &pix);
protected:
    KScanDevice *sane_device, *m_device;
    QGridLayout *layout;
    QPushButton *btnScan, *btnClose, *btnPreview, *btnSelectAll;
    LightPreviewWidget *previewPicture;
    QProgressBar *prgStatus;
protected slots:
    void imageReceived(QImage *, ImgScanInfo *);
    void previewReceived(QImage *, ImgScanInfo *);
    void scanClicked();
    void previewClicked();
};
