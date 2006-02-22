#ifndef BC_H
#define BC_H

#include "interfaces.h"
#include "ui_blurwidget.h"

class BlurWidget;

class Blur : public QObject, public PluginInterface, public ToolInterface {
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
    QPointer < BlurWidget > settingsToolBar;
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

class BlurWidget : public QWidget, public Ui::BlurWidget {
    friend class Blur;
    public:
        BlurWidget(QWidget *parent) : QWidget(parent) {
            setupUi(this);
        }
};

#endif

