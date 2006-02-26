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

#include "blur.h"
#include "plugin_functions.h"
#include <cassert>
#include <algorithm>

bool Blur::previewCheckedByDefault = true;

void Blur::init(QString &fileName, QObject *parent) {
    this->fileName = fileName;
    this->parent = parent;
    updateTimer.setSingleShot(true);
    updateTimer.setInterval(750);
    connect(&updateTimer, SIGNAL(timeout()), SLOT(updatePreview()));
}

QImage *Blur::activate(QPixmap *pix) {
    QImage dst(pix->toImage());
    dst.detach();
    QImage src(dst.convertToFormat(QImage::Format_ARGB32));
    int strength = settingsToolBar->sldStrength->value();
    ConvolutionMatrix matrix(3, 3);
    matrix.set(0, 0, 1);
    matrix.set(0, 1, 2);
    matrix.set(0, 2, 1);
    matrix.set(1, 0, 2);
    int istrength = 11-strength;
    matrix.set(1, 1, istrength*istrength/3);
    matrix.set(1, 2, 2);
    matrix.set(2, 0, 1);
    matrix.set(2, 1, 2);
    matrix.set(2, 2, 1);
    ImageConvolutionMatrix icMatrix(matrix);
    for (int x=0;x<dst.width();x++) {
        for (int y=0;y<dst.height();y++) {
            icMatrix.apply(dst, src, x, y);
        }
    }
    return new QImage(dst);
}

QImage *Blur::activate(QImage *img) {
    return NULL;
}

QWidget *Blur::getSettingsToolBar(QPixmap *pix) {
    if (settingsToolBar) {
        assert(0); //The settings toolbar already exists. It should be deleted first.
    }
    assert(pix);
    this->pix = pix;
    settingsToolBar = new BlurWidget(NULL);
    settingsToolBar->chkPreview->setChecked(previewCheckedByDefault);
    connect(new QShortcut(QKeySequence("Return"), settingsToolBar->btnOk), SIGNAL(activated()),
            settingsToolBar->btnOk, SIGNAL(clicked()));
    connect(new QShortcut(QKeySequence("Enter"), settingsToolBar->btnOk), SIGNAL(activated()),
            settingsToolBar->btnOk, SIGNAL(clicked()));
    connect(new QShortcut(QKeySequence("Esc"), settingsToolBar->btnCancel),
            SIGNAL(activated()), settingsToolBar->btnCancel, SIGNAL(clicked()));
    connect(settingsToolBar->btnOk, SIGNAL(clicked()), SLOT(okClicked()));
    connect(settingsToolBar->btnCancel, SIGNAL(clicked()), SLOT(cancelClicked()));
    connect(settingsToolBar->chkPreview, SIGNAL(toggled(bool)), SLOT(previewToggled(bool)));
    connect(settingsToolBar->sldStrength, SIGNAL(valueChanged(int)),
            &updateTimer, SLOT(start()));
    connect(settingsToolBar, SIGNAL(destroyed()), SLOT(sendPreviewOff()));
    updateTimer.start();
    return settingsToolBar;
}

QString Blur::getInternalName() {
    return QString::fromLatin1("blur");
}

PixmapViewTool *Blur::getViewTool() {
    return NULL;
}

QListWidgetItem *Blur::createListEntry(QListWidget *parent) {
    //take the abs path of library and get the icon in the same directory
    QDir thisDir(QFileInfo(fileName).absoluteDir());
    QString name(thisDir.filePath("blur.png"));
    QIcon icon(name);
    QListWidgetItem *item = new QListWidgetItem(tr("Blur"), parent);
    item->setToolTip(tr("Blur - u"));
    item->setIcon(icon);
    item->setFlags(Qt::ItemIsEnabled);
    return item;
}

QKeySequence Blur::getShortcutSequence() {
    return QKeySequence(tr("u", "blur tool"));
}

void Blur::okClicked() {
    AddChangeToolEvent *event = new AddChangeToolEvent;
    QImage *img = activate(pix);
    event->pixmap = new QPixmap(QPixmap::fromImage(*img));
    int strength = settingsToolBar->sldStrength->value();
    QString text;
    delete img;
    event->changeDesc = tr("Blur at Strength %1").arg(strength);
    QCoreApplication::sendEvent(parent, event);
    delete pix;
    delete settingsToolBar;
}

void Blur::cancelClicked() {
    sendPreviewOff();
    delete pix;
    delete settingsToolBar;
}

void Blur::previewToggled(bool checked) {
    previewCheckedByDefault = checked;
    updatePreview(settingsToolBar->chkPreview->isChecked());
}

void Blur::updatePreview(bool checked) {
    if (checked && settingsToolBar) {
        sendPreviewOn();
    } else {
        sendPreviewOff();
    }
}

void Blur::updatePreview() {
    updatePreview(settingsToolBar->chkPreview->isChecked());
}

void Blur::sendPreviewOn() {
    if (pix) {
        UpdatePreviewToolEvent *event = new UpdatePreviewToolEvent;
        QImage *img = activate(pix);
        event->pixmap = new QPixmap(QPixmap::fromImage(*img));
        delete img;
        QCoreApplication::sendEvent(parent, event);
    }
}

void Blur::sendPreviewOff() {
    if (pix) {
        UpdatePreviewToolEvent *event = new UpdatePreviewToolEvent;
        QCoreApplication::sendEvent(parent, event);
        updateTimer.stop();
    }
}

Q_EXPORT_PLUGIN(Blur)
