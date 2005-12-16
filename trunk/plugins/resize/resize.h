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

#ifndef RESIZE_H
#define RESIZE_H

#include "PixmapView.h"
#include "interfaces.h"
#include "PixmapViewTool.h"
#include "ui_resizewidget.h"

class ResizeWidget;

class Resize : public QObject, public PluginInterface, public ToolInterface {
Q_OBJECT
Q_INTERFACES(PluginInterface ToolInterface)
public:
    virtual void init(QString &fileName, lua_State *L, QObject *parent);
    virtual QImage *activate(QPixmap *pix);
    virtual QImage *activate(QImage *img);
    virtual QWidget *getSettingsToolBar(QPixmap *pix);
    virtual PixmapViewTool *getViewTool();
    virtual QKeySequence getShortcutSequence();
    virtual QString getInternalName();
    virtual QListWidgetItem *createListEntry(QListWidget *parent);
protected:
    QPointer < ResizeWidget > settingsToolBar;
    QString fileName;
    QPixmap *pix;
    QObject *parent;
    bool linked, locked;
protected slots:
    void okClicked();
    void cancelClicked();
    void linkClicked();
    
    void pixWidthChanged(int);
    void pixHeightChanged(int);
    void pctWidthChanged(int);
    void pctHeightChanged(int);
signals:
    void valueChanged(QSize);
};

class ResizeWidget : public QWidget, public Ui::ResizeWidget {
public:
    ResizeWidget(QWidget *parent) : QWidget(parent) {
        setupUi(this);
    }
};

#endif

