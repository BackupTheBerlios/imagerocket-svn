#ifndef GAMMA_H
#define GAMMA_H

#include "interfaces.h"
#include "ui_gammawidget.h"

class GammaWidget;

class Gamma : public QObject, public PluginInterface, public ToolInterface {
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
    QPointer < GammaWidget > settingsToolBar;
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

class GammaWidget : public QWidget, public Ui::GammaWidget {
    friend class Gamma;
    public:
        GammaWidget(QWidget *parent) : QWidget(parent) {
            setupUi(this);
        }
};

#endif

