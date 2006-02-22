#ifndef BC_H
#define BC_H

#include "interfaces.h"
#include "ui_sharpenwidget.h"

class SharpenWidget;

class Sharpen : public QObject, public PluginInterface, public ToolInterface {
Q_OBJECT
Q_INTERFACES(PluginInterface ToolInterface)
public:
    virtual void init(QString &fileName, QObject *parent);
    virtual QImage *activate(QPixmap *pix);
    virtual QImage *activate(QImage *img);
    virtual QWidget *getSettingsToolBar(QPixmap *pix);
    virtual PixmapViewTool *getViewTool();
    virtual QString getInternalName();
    virtual QListWidgetItem *createListEntry(QListWidget *parent);
    virtual QKeySequence getShortcutSequence();
protected:
    QPointer < SharpenWidget > settingsToolBar;
    QString fileName;
    QPixmap *pix;
    QObject *parent;
    QTimer updateTimer;
    void run();
    static bool previewCheckedByDefault;
protected slots:
    void okClicked();
    void cancelClicked();
    void previewToggled(bool);
    void updatePreview(bool checked);
    void updatePreview();
    void sendPreviewOn();
    void sendPreviewOff();
};

class SharpenWidget : public QWidget, public Ui::SharpenWidget {
    friend class Sharpen;
    public:
        SharpenWidget(QWidget *parent) : QWidget(parent) {
            setupUi(this);
        }
};

#endif

