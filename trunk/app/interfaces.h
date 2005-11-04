#ifndef PLUGIN_INTERFACE_H
#define PLUGIN_INTERFACE_H

class PixmapViewTool;
#include <QtGui>
extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

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
        AddChangeToolEvent() : QEvent(static_cast < QEvent::Type > (QEvent::User+1)) {
            pixmap = NULL;
        }
};

class PluginInterface {
public:
    virtual void init(QString &fileName, lua_State *L, QObject *parent) = 0;
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
    virtual int length() = 0;
    virtual void reset() = 0;
    virtual ~ToolInterface() {}
};

Q_DECLARE_INTERFACE(ToolInterface,
                    "crossman.wesley.ImageRocket.ToolInterface/1.0")
#endif
