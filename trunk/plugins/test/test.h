#ifndef TEST_H
#define TEST_H

#include "interfaces.h"

class Test : public QObject, public PluginInterface, public ToolInterface {
Q_OBJECT
Q_INTERFACES(PluginInterface ToolInterface)
protected:
    QString fileName;
    QPixmap *pix;
    void run();
public:
    virtual void init(QString &fileName, lua_State *L, QVector < PluginInterface * > &plugins);
    virtual QImage *activate(QPixmap *pix);
    virtual QImage *activate(QImage *img);
    virtual void reset();
    virtual int length();
    QListWidgetItem *createListEntry(QListWidget *parent);
    
};

#endif
