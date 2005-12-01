#ifndef BC_H
#define BC_H

#include "interfaces.h"
#include "ui_bcwidget.h"

class BrightnessContrastWidget;

class BrightnessContrast : public QObject, public PluginInterface, public ToolInterface {
Q_OBJECT
Q_INTERFACES(PluginInterface ToolInterface)
public:
    virtual void init(QString &fileName, lua_State *L, QObject *parent);
    virtual QImage *activate(QPixmap *pix);
    virtual QImage *activate(QImage *img);
    virtual QWidget *getSettingsToolBar(QPixmap *pix);
    virtual PixmapViewTool *getViewTool();
    virtual void reset();
    virtual int length();
    virtual QListWidgetItem *createListEntry(QListWidget *parent);
    virtual QKeySequence getShortcutSequence();
protected:
    QPointer < BrightnessContrastWidget > settingsToolBar;
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

class BrightnessContrastWidget : public QWidget, public Ui::BrightnessContrastWidget {
    friend class BrightnessContrast;
    public:
        BrightnessContrastWidget(QWidget *parent) : QWidget(parent) {
            setupUi(this);
        }
};

#endif

