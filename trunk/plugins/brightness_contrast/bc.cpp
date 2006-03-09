/* ImageRocket
An image-editing program written for editing speed and ease of use.
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

#include "bc.h"
#include "ImageTools.h"
#include <cassert>
#include <algorithm>

bool BrightnessContrast::previewCheckedByDefault = true;

void BrightnessContrast::init(QString &fileName, QObject *parent) {
    this->fileName = fileName;
    this->parent = parent;
    updateTimer.setSingleShot(true);
    updateTimer.setInterval(600);
    connect(&updateTimer, SIGNAL(timeout()), SLOT(updatePreview()));
}

QImage *BrightnessContrast::activate(QPixmap *pix) {
    QImage img(pix->toImage());
    img.detach();
    assert(img.depth() == 32);
    double contrast = (100.0 + settingsToolBar->sldContrast->value()) / 100;
    contrast *= contrast;
    int brightness = settingsToolBar->sldBrightness->value();
    bool usingAlphaPm = (img.format() == QImage::Format_ARGB32_Premultiplied);
    for (int y=0;y<img.height();++y) {
        uint *line = reinterpret_cast< uint * >(img.scanLine(y));
        for (int x=0;x<img.width();++x) {
            uint *pixel = line + x;
            uint nonPmPixel = usingAlphaPm ? ImageTools::decodePremultiplied(*pixel) : *pixel;
            int arr[] = {qRed(nonPmPixel), qGreen(nonPmPixel), qBlue(nonPmPixel)};
            for (int a=0;a<3;a++) {
                int tmp = arr[a] + brightness;
                tmp = (int)(((tmp - 255 / 2 ) * contrast) + 255 / 2);
                arr[a] = std::max(0, std::min(255, tmp));
            }
            if (usingAlphaPm) {
                *pixel = ImageTools::premultiply(qRgba(arr[0], arr[1], arr[2], qAlpha(nonPmPixel)));
            } else {
                *pixel = qRgb(arr[0], arr[1], arr[2]);
            }
        }
    }
    return new QImage(img);
}

QImage *BrightnessContrast::activate(QImage *img) {
    return NULL;
}

QWidget *BrightnessContrast::getSettingsToolBar(QPixmap *pix) {
    if (settingsToolBar) {
        assert(0); //The settings toolbar already exists. It should be deleted first.
    }
    assert(pix);
    this->pix = pix;
    settingsToolBar = new BrightnessContrastWidget(NULL);
    connect(new QShortcut(QKeySequence("Return"), settingsToolBar->btnOk), SIGNAL(activated()),
            settingsToolBar->btnOk, SIGNAL(clicked()));
    connect(new QShortcut(QKeySequence("Enter"), settingsToolBar->btnOk), SIGNAL(activated()),
            settingsToolBar->btnOk, SIGNAL(clicked()));
    connect(new QShortcut(QKeySequence("Esc"), settingsToolBar->btnCancel),
            SIGNAL(activated()), settingsToolBar->btnCancel, SIGNAL(clicked()));
    connect(settingsToolBar->btnOk, SIGNAL(clicked()), SLOT(okClicked()));
    connect(settingsToolBar->btnCancel, SIGNAL(clicked()), SLOT(cancelClicked()));
    connect(settingsToolBar->chkPreview, SIGNAL(toggled(bool)), SLOT(previewToggled(bool)));
    connect(settingsToolBar->sldBrightness, SIGNAL(valueChanged(int)),
            &updateTimer, SLOT(start()));
    connect(settingsToolBar->sldContrast, SIGNAL(valueChanged(int)),
            &updateTimer, SLOT(start()));
    connect(settingsToolBar, SIGNAL(destroyed()), SLOT(sendPreviewOff()));
    settingsToolBar->chkPreview->setChecked(previewCheckedByDefault);
    return settingsToolBar;
}

QString BrightnessContrast::getInternalName() {
    return QString::fromLatin1("brightnesscontrast");
}

PixmapViewTool *BrightnessContrast::getViewTool() {
    return NULL;
}

QListWidgetItem *BrightnessContrast::createListEntry(QListWidget *parent) {
    //take the abs path of library and get the icon in the same directory
    QDir thisDir(QFileInfo(fileName).absoluteDir());
    QString name(thisDir.filePath("bc.png"));
    QIcon icon(name);
    QListWidgetItem *item = new QListWidgetItem(tr("Bright/Contrast"), parent);
    item->setToolTip(tr("Brightness/Contrast - b"));
    item->setIcon(icon);
    item->setFlags(Qt::ItemIsEnabled);
    return item;
}

QKeySequence BrightnessContrast::getShortcutSequence() {
    return QKeySequence(tr("b", "brightness/contrast tool"));
}

void BrightnessContrast::okClicked() {
    AddChangeToolEvent *event = new AddChangeToolEvent;
    QImage *img = activate(pix);
    event->pixmap = new QPixmap(QPixmap::fromImage(*img));
    int bright = settingsToolBar->sldBrightness->value();
    int contrast = settingsToolBar->sldContrast->value();
    QString text;
    delete img;
    if (bright && contrast) {
        event->changeDesc = tr("Brighten %1/Contrast %2").arg(bright).arg(contrast);
        QCoreApplication::sendEvent(parent, event);
    } else if (bright) {
        event->changeDesc = tr("Brighten %1").arg(bright);
        QCoreApplication::sendEvent(parent, event);
    } else if (contrast) {
        event->changeDesc = tr("Contrast %1").arg(contrast);
        QCoreApplication::sendEvent(parent, event);
    }
    delete pix;
    delete settingsToolBar;
}

void BrightnessContrast::cancelClicked() {
    sendPreviewOff();
    delete pix;
    delete settingsToolBar;
}

void BrightnessContrast::previewToggled(bool checked) {
    previewCheckedByDefault = checked;
    updatePreview(settingsToolBar->chkPreview->isChecked());
}

void BrightnessContrast::updatePreview(bool checked) {
    if (checked && settingsToolBar
            && (settingsToolBar->sldBrightness->value() != 0
            || settingsToolBar->sldContrast->value() != 0)) {
        sendPreviewOn();
    } else {
        sendPreviewOff();
    }
}

void BrightnessContrast::updatePreview() {
    updatePreview(settingsToolBar->chkPreview->isChecked());
}

void BrightnessContrast::sendPreviewOn() {
    if (pix) {
        UpdatePreviewToolEvent *event = new UpdatePreviewToolEvent;
        QImage *img = activate(pix);
        event->pixmap = new QPixmap(QPixmap::fromImage(*img));
        delete img;
        QCoreApplication::sendEvent(parent, event);
    }
}

void BrightnessContrast::sendPreviewOff() {
    if (pix) {
        UpdatePreviewToolEvent *event = new UpdatePreviewToolEvent;
        QCoreApplication::sendEvent(parent, event);
        updateTimer.stop();
    }
}

Q_EXPORT_PLUGIN(BrightnessContrast)

