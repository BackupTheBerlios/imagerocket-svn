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

#ifndef PLUGIN_INTERFACE_H
#define PLUGIN_INTERFACE_H

class PixmapViewTool;
#include <QtGui>

class UpdatePreviewToolEvent : public QEvent {
public:
    QPixmap *pixmap;
    UpdatePreviewToolEvent() : QEvent(QEvent::User) {
        pixmap = NULL;
    }
};

class AddChangeToolEvent : public QEvent {
public:
    QPixmap *pixmap;
    QString changeDesc;
    AddChangeToolEvent() : QEvent(static_cast < QEvent::Type > (QEvent::User+1)) {
        pixmap = NULL;
    }
};

class PluginInterface {
public:
    virtual void init(QString &fileName, QObject *parent) = 0;
    virtual ~PluginInterface() {}
};

Q_DECLARE_INTERFACE(PluginInterface,
                    "crossman.wesley.ImageRocket.PluginInterface/1.0")

class ToolInterface {
public:
    virtual QListWidgetItem *createListEntry(QListWidget *parent) = 0;
    virtual QImage *activate(QPixmap *pix = NULL) = 0;
    virtual QImage *activate(QImage *img = NULL) = 0;
    virtual QWidget *getSettingsToolBar(QPixmap *pix) = 0;
    virtual PixmapViewTool *getViewTool() = 0;
    virtual QKeySequence getShortcutSequence() = 0;
    virtual QString getInternalName() = 0;
    virtual ~ToolInterface() {}
};

Q_DECLARE_INTERFACE(ToolInterface,
                    "crossman.wesley.ImageRocket.ToolInterface/1.0")
#endif
