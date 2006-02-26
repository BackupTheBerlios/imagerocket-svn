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

#ifndef BC_H
#define BC_H

#include "interfaces.h"
#include "ui_bcwidget.h"

class BrightnessContrastWidget;

class BrightnessContrast : public QObject, public PluginInterface, public ToolInterface {
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

