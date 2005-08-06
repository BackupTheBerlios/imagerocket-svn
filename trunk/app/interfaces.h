#ifndef PLUGIN_INTERFACE_H
#define PLUGIN_INTERFACE_H

#include <QtGui>
extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

class PluginInterface {
public:
    virtual void init(QString &fileName, lua_State *L, QVector < PluginInterface * > &plugins) = 0;
    virtual ~PluginInterface() {}
};

Q_DECLARE_INTERFACE(PluginInterface,
                    "crossman.wesley.ImageRocket.PluginInterface/1.0")

class ToolInterface {
public:
    virtual QListWidgetItem *createListEntry(QListWidget *parent) = 0;
    virtual QImage *activate(QPixmap *pix = NULL) = 0;
    virtual QImage *activate(QImage *img = NULL) = 0;
    virtual int length() = 0;
    virtual void reset() = 0;
    virtual ~ToolInterface() {}
};

Q_DECLARE_INTERFACE(ToolInterface,
                    "crossman.wesley.ImageRocket.ToolInterface/1.0")
#endif
