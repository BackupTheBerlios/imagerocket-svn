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
    QPixmap tmp(pix->copy(w->spnX->value(), w->spnY->value(), w->spnW->value(), w->spnH->value()));
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
    settingsToolBar->spnX->setRange(0, pix->width()-1);
    settingsToolBar->spnY->setRange(0, pix->height()-1);
    settingsToolBar->spnW->setRange(1, pix->width());
    settingsToolBar->spnH->setRange(1, pix->height());
    settingsToolBar->setMinimumWidth(1);
    selectionChanged(ImageRect());
    return settingsToolBar;
}

PixmapViewTool *Crop::getViewTool() {
    CropViewTool *tool = new CropViewTool;
    connect(settingsToolBar, SIGNAL(destroyed()), tool, SLOT(deleteLater()));
    connect(tool, SIGNAL(selectionChanged(ImageRect)), SLOT(selectionChanged(ImageRect)));
    connect(tool, SIGNAL(selected()), settingsToolBar->btnOk, SIGNAL(clicked()));
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

void Crop::selectionChanged(ImageRect ir) {
    if (!ir.isNull()) {
        settingsToolBar->spnX->setValue(ir.x());
        settingsToolBar->spnY->setValue(ir.y());
        settingsToolBar->spnW->setValue(ir.width());
        settingsToolBar->spnH->setValue(ir.height());
    }
    settingsToolBar->stackedLayout->setCurrentIndex(ir.isNull() ? 0 : 1);
}

Q_EXPORT_PLUGIN(Crop)

