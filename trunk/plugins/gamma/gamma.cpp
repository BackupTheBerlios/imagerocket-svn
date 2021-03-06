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

#include "gamma.h"
#include "ImageTools.h"
#include <cassert>
#include <cmath>
#include <algorithm>

#define EQUALS(a, b) ((a-.0001<b) && (a+.0001>b))

bool Gamma::previewCheckedByDefault = true;

void Gamma::init(QString &fileName, QObject *parent) {
    this->fileName = fileName;
    this->parent = parent;
    updateTimer.setSingleShot(true);
    updateTimer.setInterval(750);
    connect(&updateTimer, SIGNAL(timeout()), SLOT(updatePreview()));
}

QImage *Gamma::activate(QPixmap *pix) {
    QImage img(pix->toImage());
    img.detach();
    assert(img.depth() == 32);
    double master = settingsToolBar->spnMaster->value();
    ImageTools::GammaTable color[] = {
            ImageTools::GammaTable(master * settingsToolBar->spnRed->value()),
            ImageTools::GammaTable(master * settingsToolBar->spnGreen->value()),
            ImageTools::GammaTable(master * settingsToolBar->spnBlue->value())
    };
    bool usingAlphaPm = (img.format() == QImage::Format_ARGB32_Premultiplied);
    for (int y=0;y<img.height();++y) {
        uint *line = reinterpret_cast< uint * >(img.scanLine(y));
        for (int x=0;x<img.width();++x) {
            uint *pixel = line + x;
            uint nonPmPixel = usingAlphaPm ? ImageTools::decodePremultiplied(*pixel) : *pixel;
            int arr[] = {qRed(nonPmPixel), qGreen(nonPmPixel), qBlue(nonPmPixel)};
            for (int a=0;a<3;a++) {
                arr[a] = color[a][arr[a]];
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

QImage *Gamma::activate(QImage *img) {
    return NULL;
}

QWidget *Gamma::getSettingsToolBar(QPixmap *pix) {
    if (settingsToolBar) {
        assert(0); //The settings toolbar already exists. It should be deleted first.
    }
    assert(pix);
    this->pix = pix;
    settingsToolBar = new GammaWidget(NULL);
    connect(new QShortcut(QKeySequence("Return"), settingsToolBar->btnOk), SIGNAL(activated()),
            settingsToolBar->btnOk, SIGNAL(clicked()));
    connect(new QShortcut(QKeySequence("Enter"), settingsToolBar->btnOk), SIGNAL(activated()),
            settingsToolBar->btnOk, SIGNAL(clicked()));
    connect(new QShortcut(QKeySequence("Esc"), settingsToolBar->btnCancel),
            SIGNAL(activated()), settingsToolBar->btnCancel, SIGNAL(clicked()));
    connect(settingsToolBar->btnOk, SIGNAL(clicked()), SLOT(okClicked()));
    connect(settingsToolBar->btnCancel, SIGNAL(clicked()), SLOT(cancelClicked()));
    connect(settingsToolBar->chkPreview, SIGNAL(toggled(bool)), SLOT(previewToggled(bool)));
    connect(settingsToolBar->spnMaster, SIGNAL(valueChanged(double)),
            &updateTimer, SLOT(start()));
    connect(settingsToolBar->spnRed, SIGNAL(valueChanged(double)),
            &updateTimer, SLOT(start()));
    connect(settingsToolBar->spnGreen, SIGNAL(valueChanged(double)),
            &updateTimer, SLOT(start()));
    connect(settingsToolBar->spnBlue, SIGNAL(valueChanged(double)),
            &updateTimer, SLOT(start()));
    connect(settingsToolBar, SIGNAL(destroyed()), SLOT(sendPreviewOff()));
    settingsToolBar->chkPreview->setChecked(previewCheckedByDefault);
    return settingsToolBar;
}

QString Gamma::getInternalName() {
    return QString::fromLatin1("gamma");
}

PixmapViewTool *Gamma::getViewTool() {
    return NULL;
}

QListWidgetItem *Gamma::createListEntry(QListWidget *parent) {
    //take the abs path of library and get the icon in the same directory
    QDir thisDir(QFileInfo(fileName).absoluteDir());
    QString name(thisDir.filePath("gamma.png"));
    QIcon icon(name);
    QListWidgetItem *item = new QListWidgetItem(tr("Gamma Adjust"), parent);
    item->setToolTip(tr("Gamma Adjust - g"));
    item->setIcon(icon);
    item->setFlags(Qt::ItemIsEnabled);
    return item;
}

QKeySequence Gamma::getShortcutSequence() {
    return QKeySequence(tr("g", "gamma adjust tool"));
}

void Gamma::okClicked() {
    AddChangeToolEvent *event = new AddChangeToolEvent;
    QImage *img = activate(pix);
    event->pixmap = new QPixmap(QPixmap::fromImage(*img));
    delete img;
    double m = settingsToolBar->spnMaster->value();
    double r = settingsToolBar->spnRed->value();
    double g = settingsToolBar->spnGreen->value();
    double b = settingsToolBar->spnBlue->value();
    if (!EQUALS(r, 1.0) || !EQUALS(g, 1.0) || !EQUALS(b, 1.0)) {
        event->changeDesc = tr("Gamma Adjust M%1 / R%2 / G%3 / B%4").arg(m).arg(r).arg(g).arg(b);
        QCoreApplication::sendEvent(parent, event);
    } else if (!EQUALS(m, 1.0)) {
        event->changeDesc = tr("Gamma Adjust %1").arg(m);
        QCoreApplication::sendEvent(parent, event);
    }
    delete pix;
    delete settingsToolBar;
}

void Gamma::cancelClicked() {
    sendPreviewOff();
    delete pix;
    delete settingsToolBar;
}

void Gamma::previewToggled(bool checked) {
    previewCheckedByDefault = checked;
    updatePreview(settingsToolBar->chkPreview->isChecked());
}

void Gamma::updatePreview(bool checked) {
    if (checked && settingsToolBar
            && (!EQUALS(settingsToolBar->spnMaster->value(), 1.0)
            || !EQUALS(settingsToolBar->spnRed->value(), 1.0)
            || !EQUALS(settingsToolBar->spnGreen->value(), 1.0)
            || !EQUALS(settingsToolBar->spnBlue->value(), 1.0))) {
        sendPreviewOn();
    } else {
        sendPreviewOff();
    }
}

void Gamma::updatePreview() {
    updatePreview(settingsToolBar->chkPreview->isChecked());
}

void Gamma::sendPreviewOn() {
    if (pix) {
        UpdatePreviewToolEvent *event = new UpdatePreviewToolEvent;
        QImage *img = activate(pix);
        event->pixmap = new QPixmap(QPixmap::fromImage(*img));
        delete img;
        QCoreApplication::sendEvent(parent, event);
    }
}

void Gamma::sendPreviewOff() {
    if (pix) {
        UpdatePreviewToolEvent *event = new UpdatePreviewToolEvent;
        QCoreApplication::sendEvent(parent, event);
        updateTimer.stop();
    }
}

Q_EXPORT_PLUGIN(Gamma)

