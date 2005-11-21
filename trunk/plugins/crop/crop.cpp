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

#include "crop.h"
#include "CropViewTool.h"
#include "PixmapViewTool.h"
#include "PixmapView.h"
#include <cassert>
#include <algorithm>

void Crop::init(QString &fileName, lua_State *L, QObject *parent) {
    this->fileName = fileName;
    this->parent = parent;
    
    updating = false;
    
    QFile script(":/crop.lua");
    script.open(QFile::ReadOnly);
    QTextStream in(&script);
    QString scriptStr(in.readAll());
    if (luaL_loadbuffer(L, scriptStr.toAscii(), scriptStr.length(), "<lua>")
        || lua_pcall(L, 0, 0, 0)) {
        lua_pushfstring(L, "Error loading bc.lua -- %s",
                        lua_tostring(L, -1));
        lua_error(L);
    }
}

QImage *Crop::activate(QPixmap *pix) {
    CropWidget *w = settingsToolBar;
    QRect r(w->spnX->value(), w->spnY->value(), w->spnW->value(), w->spnH->value());
    assert(r.x() >= 0 && r.y() >= 0 && pix->width() > r.right() && pix->height() > r.bottom());
    QPixmap tmp(pix->copy(r));
    return new QImage(tmp.toImage());
}

QImage *Crop::activate(QImage *img) {
    return NULL;
}

QWidget *Crop::getSettingsToolBar(QPixmap *pix) {
    if (settingsToolBar) {
        assert(0); //The settings toolbar already exists. It should be deleted first.
    }
    assert(pix);
    this->pix = pix;
    settingsToolBar = new CropWidget(NULL);
    connect(new QShortcut(QKeySequence("Return"), settingsToolBar->btnOk), SIGNAL(activated()),
            settingsToolBar->btnOk, SIGNAL(clicked()));
    connect(new QShortcut(QKeySequence("Enter"), settingsToolBar->btnOk), SIGNAL(activated()),
            settingsToolBar->btnOk, SIGNAL(clicked()));
    connect(new QShortcut(QKeySequence("Esc"), settingsToolBar->btnCancel),
            SIGNAL(activated()), settingsToolBar->btnCancel, SIGNAL(clicked()));
    connect(settingsToolBar->btnOk, SIGNAL(clicked()), SLOT(okClicked()));
    connect(settingsToolBar->btnCancel, SIGNAL(clicked()), SLOT(cancelClicked()));
    connect(settingsToolBar->btnCancel2, SIGNAL(clicked()), SLOT(cancelClicked()));
    connect(settingsToolBar->spnX, SIGNAL(valueChanged(int)), SLOT(spinBoxesValueChanged()));
    connect(settingsToolBar->spnY, SIGNAL(valueChanged(int)), SLOT(spinBoxesValueChanged()));
    connect(settingsToolBar->spnW, SIGNAL(valueChanged(int)), SLOT(spinBoxesValueChanged()));
    connect(settingsToolBar->spnH, SIGNAL(valueChanged(int)), SLOT(spinBoxesValueChanged()));
    settingsToolBar->setMinimumWidth(1);
    selectionChanged(ImageRect());
    return settingsToolBar;
}

void Crop::spinBoxesValueChanged() {
    if (!updating) {
        updating = true;
        ImageRect ir(settingsToolBar->spnX->value(), settingsToolBar->spnY->value(),
                settingsToolBar->spnW->value(), settingsToolBar->spnH->value());
        emit selectionChanged(ir);
        updateMaximumValues(ir);
        updating = false;
    }
}

PixmapViewTool *Crop::getViewTool() {
    CropViewTool *tool = new CropViewTool;
    connect(settingsToolBar, SIGNAL(destroyed()), tool, SLOT(deleteLater()));
    connect(tool, SIGNAL(selectionChanged(ImageRect)), SLOT(selectionWasChanged(ImageRect)));
    connect(tool, SIGNAL(selected()), settingsToolBar->btnOk, SIGNAL(clicked()));
    connect(this, SIGNAL(selectionChanged(ImageRect)), tool, SLOT(setSelection(ImageRect)));
    return tool;
}

int Crop::length() {
    return 1;
}

void Crop::reset() {
}

QListWidgetItem *Crop::createListEntry(QListWidget *parent) {
    //take the abs path of library and get the icon in the same directory
    QDir thisDir(QFileInfo(fileName).absoluteDir());
    QString name(thisDir.filePath("crop.png"));
    QIcon icon(name);
    QListWidgetItem *item = new QListWidgetItem(tr("Crop"), parent);
    item->setIcon(icon);
    item->setFlags(Qt::ItemIsEnabled);
    return item;
}

void Crop::okClicked() {
    AddChangeToolEvent *event = new AddChangeToolEvent;
    QImage *img = activate(pix);
    event->pixmap = new QPixmap(QPixmap::fromImage(*img));
    delete img;
    QCoreApplication::sendEvent(parent, event);
    delete pix;
    delete settingsToolBar;
}

void Crop::cancelClicked() {
    delete pix;
    delete settingsToolBar;
}

void Crop::updateMaximumValues(ImageRect ir) {
    int x = pix->width()-ir.width();
    if (x != settingsToolBar->spnX->maximum()) settingsToolBar->spnX->setMaximum(x);
    int y = pix->height()-ir.height();
    if (y != settingsToolBar->spnY->maximum()) settingsToolBar->spnY->setMaximum(y);
    int w = pix->width()-ir.x();
    if (w != settingsToolBar->spnW->maximum()) settingsToolBar->spnW->setMaximum(w);
    int h = pix->height()-ir.y();
    if (h != settingsToolBar->spnH->maximum()) settingsToolBar->spnH->setMaximum(h);
}

void Crop::selectionWasChanged(ImageRect ir) {
    if (!ir.isNull() && !updating) {
        updating = true;
        updateMaximumValues(ir);
        settingsToolBar->spnX->setValue(ir.x());
        settingsToolBar->spnY->setValue(ir.y());
        settingsToolBar->spnW->setValue(ir.width());
        settingsToolBar->spnH->setValue(ir.height());
        updating = false;
    }
    settingsToolBar->stackedLayout->setCurrentIndex(ir.isNull() ? 0 : 1);
}

Q_EXPORT_PLUGIN(Crop)

