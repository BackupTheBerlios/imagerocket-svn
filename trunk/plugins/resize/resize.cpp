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

#include "resize.h"
#include "ResizeViewTool.h"
#include "PixmapViewTool.h"
#include "PixmapView.h"
#include <cassert>
#include <algorithm>

void Resize::init(QString &fileName, QObject *parent) {
    this->fileName = fileName;
    this->parent = parent;
    locked = false;
}

QImage *Resize::activate(QPixmap *pix) {
    ResizeWidget *w = settingsToolBar;
    QPixmap tmp(pix->scaled(settingsToolBar->spnPixWidth->value(),
            settingsToolBar->spnPixHeight->value(), Qt::IgnoreAspectRatio,
            Qt::SmoothTransformation));
    return new QImage(tmp.toImage());
}

QImage *Resize::activate(QImage *img) {
    return NULL;
}

QWidget *Resize::getSettingsToolBar(QPixmap *pix) {
    if (settingsToolBar) {
        assert(0); //The settings toolbar already exists. It should be deleted first.
    }
    assert(pix);
    this->pix = pix;
    linked = true;
    settingsToolBar = new ResizeWidget(NULL);
    settingsToolBar->spnPixWidth->setValue(pix->width());
    settingsToolBar->spnPixHeight->setValue(pix->height());
    connect(new QShortcut(QKeySequence("Return"), settingsToolBar->btnOk), SIGNAL(activated()),
            settingsToolBar->btnOk, SIGNAL(clicked()));
    connect(new QShortcut(QKeySequence("Enter"), settingsToolBar->btnOk), SIGNAL(activated()),
            settingsToolBar->btnOk, SIGNAL(clicked()));
    //Both of the following connections seem to be necessary, since they don't seem to
    //trigger when their recipient is invisible. I guess this is safe. - WJC
    connect(new QShortcut(QKeySequence("Esc"), settingsToolBar->btnCancel),
            SIGNAL(activated()), settingsToolBar->btnCancel, SIGNAL(clicked()));
    connect(settingsToolBar->btnOk, SIGNAL(clicked()), SLOT(okClicked()));
    connect(settingsToolBar->btnCancel, SIGNAL(clicked()), SLOT(cancelClicked()));
    connect(settingsToolBar->btnLink, SIGNAL(clicked()), SLOT(linkClicked()));
    
    connect(settingsToolBar->spnPixWidth, SIGNAL(valueChanged(int)), SLOT(pixWidthChanged(int)));
    connect(settingsToolBar->spnPixHeight, SIGNAL(valueChanged(int)), SLOT(pixHeightChanged(int)));
    connect(settingsToolBar->spnPctWidth, SIGNAL(valueChanged(int)), SLOT(pctWidthChanged(int)));
    connect(settingsToolBar->spnPctHeight, SIGNAL(valueChanged(int)), SLOT(pctHeightChanged(int)));
    settingsToolBar->setMinimumWidth(1);
    return settingsToolBar;
}

PixmapViewTool *Resize::getViewTool() {
    ResizeViewTool *tool = new ResizeViewTool;
    connect(settingsToolBar, SIGNAL(destroyed()), tool, SLOT(deleteLater()));
    connect(this, SIGNAL(valueChanged(QSize)), tool, SLOT(setSize(QSize)));
    tool->setSize(QSize(pix->width(), pix->height()));
    return tool;
}

QString Resize::getInternalName() {
    return QString::fromLatin1("resize");
}

QListWidgetItem *Resize::createListEntry(QListWidget *parent) {
    //take the abs path of library and get the icon in the same directory
    QDir thisDir(QFileInfo(fileName).absoluteDir());
    QString name(thisDir.filePath("resize.png"));
    QIcon icon(name);
    QListWidgetItem *item = new QListWidgetItem(tr("Resize"), parent);
    item->setToolTip(tr("Resize - r"));
    item->setIcon(icon);
    item->setFlags(Qt::ItemIsEnabled);
    return item;
}

QKeySequence Resize::getShortcutSequence() {
    return QKeySequence(tr("r", "resize tool"));
}

void Resize::okClicked() {
    AddChangeToolEvent *event = new AddChangeToolEvent;
    QImage *img = activate(pix);
    event->pixmap = new QPixmap(QPixmap::fromImage(*img));
    event->changeDesc = tr("Resize");
    delete img;
    QCoreApplication::sendEvent(parent, event);
    delete pix;
    delete settingsToolBar;
}

void Resize::cancelClicked() {
    delete pix;
    pix = NULL;
    delete settingsToolBar;
    
}

void Resize::linkClicked() {
    if (settingsToolBar) {
        linked = !linked;
        if (linked) {
            settingsToolBar->btnLink->setIcon(QPixmap(":/linked.png"));
            settingsToolBar->btnLink->setToolTip(tr("Width and Height Linked"));
        } else {
            settingsToolBar->btnLink->setIcon(QPixmap(":/unlinked.png"));
            settingsToolBar->btnLink->setToolTip(tr("Width and Height Not Linked"));
        }
    }
}

void Resize::pixWidthChanged(int value) {
    if (settingsToolBar && !locked) {
        locked = true;
        double pct = double(value)/pix->width()*100.0;
        settingsToolBar->spnPctWidth->setValue(int(pct));
        if (linked) {
            double yRatio = double(pix->height()) / pix->width();
            settingsToolBar->spnPixHeight->setValue(int(value * yRatio));
            settingsToolBar->spnPctHeight->setValue(int(pct));
        }
        QSize size(settingsToolBar->spnPixWidth->value(),
                settingsToolBar->spnPixHeight->value());
        emit valueChanged(size);
        locked = false;
    }
}

void Resize::pixHeightChanged(int value) {
    if (settingsToolBar && !locked) {
        locked = true;
        double pct = double(value)/pix->height()*100.0;
        settingsToolBar->spnPctHeight->setValue(int(pct));
        if (linked) {
            double xRatio = double(pix->width()) / pix->height();
            settingsToolBar->spnPixWidth->setValue(int(value * xRatio));
            settingsToolBar->spnPctWidth->setValue(int(pct));
        }
        QSize size(settingsToolBar->spnPixWidth->value(),
                settingsToolBar->spnPixHeight->value());
        emit valueChanged(size);
        locked = false;
    }
}

void Resize::pctWidthChanged(int value) {
    if (settingsToolBar && !locked) {
        locked = true;
        if (linked) {
            settingsToolBar->spnPctHeight->setValue(value);
            settingsToolBar->spnPixHeight->setValue(int(pix->height()*(value*0.01)));
        }
        settingsToolBar->spnPixWidth->setValue(int(pix->width()*(value*0.01)));
        QSize size(settingsToolBar->spnPixWidth->value(),
                settingsToolBar->spnPixHeight->value());
        emit valueChanged(size);
        locked = false;
    }
}

void Resize::pctHeightChanged(int value) {
    if (settingsToolBar && !locked) {
        locked = true;
        if (linked) {
            settingsToolBar->spnPctWidth->setValue(value);
            settingsToolBar->spnPixWidth->setValue(int(pix->width()*(value*0.01)));
        }
        settingsToolBar->spnPixHeight->setValue(int(pix->height()*(value*0.01)));
        QSize size(settingsToolBar->spnPixWidth->value(),
                settingsToolBar->spnPixHeight->value());
        emit valueChanged(size);
        locked = false;
    }
}

Q_EXPORT_PLUGIN(Resize)

