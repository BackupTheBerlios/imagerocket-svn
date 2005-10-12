#ifndef TEST_H
#define TEST_H

#include "interfaces.h"

class Test : public QObject, public PluginInterface, public ToolInterface {
Q_OBJECT
Q_INTERFACES(PluginInterface ToolInterface)
protected:
    QPointer < QWidget > settingsToolBar;
    QString fileName;
    QPixmap *pix;
    QObject *parent;
    void run();
public:
    virtual void init(QString &fileName, lua_State *L, QObject *parent);
    virtual QImage *activate(QPixmap *pix);
    virtual QImage *activate(QImage *img);
    virtual QWidget *getSettingsToolBar(QPixmap *pix);
    virtual void reset();
    virtual int length();
    QListWidgetItem *createListEntry(QListWidget *parent);
protected slots:
    void okClicked();
    void cancelClicked();
};

#endif
