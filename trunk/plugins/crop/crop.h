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

#ifndef CROP_H
#define CROP_H

#include "PixmapView.h"
#include "interfaces.h"
#include "PixmapViewTool.h"
#include "ui_cropwidget.h"

class CropWidget;

class Crop : public QObject, public PluginInterface, public ToolInterface {
Q_OBJECT
Q_INTERFACES(PluginInterface ToolInterface)
public:
    virtual void init(QString &fileName, QObject *parent);
    virtual QImage *activate(QPixmap *pix);
    virtual QImage *activate(QImage *img);
    virtual QWidget *getSettingsToolBar(QPixmap *pix);
    virtual PixmapViewTool *getViewTool();
    virtual QKeySequence getShortcutSequence();
    virtual QString getInternalName();
    virtual QListWidgetItem *createListEntry(QListWidget *parent);
protected:
    QPointer < CropWidget > settingsToolBar;
    QString fileName;
    QPixmap *pix;
    QObject *parent;
    bool updating;
    void updateMaximumValues(ImageRect);
protected slots:
    void okClicked();
    void cancelClicked();
    void selectionWasChanged(ImageRect);
    void spinBoxesValueChanged();
signals:
    void selectionChanged(ImageRect);
};

class CropWidget : public QWidget, public Ui::CropWidget {
    public:
        QHBoxLayout *hboxLayout;
        QWidget *labelContainer;
        QStackedLayout *stackedLayout;
        QLabel *lblClickAndDrag;
        QPushButton *btnCancel2;
        QWidget *controlContainer;
        CropWidget(QWidget *parent) : QWidget(parent) {
            QPalette p;
            p.setColor(QPalette::Background, Qt::red);
            setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            
            //page 1
            labelContainer = new QWidget(this);
            gridLayout = new QGridLayout(labelContainer); gridLayout->setMargin(3);
            lblClickAndDrag = new QLabel(labelContainer);
            lblClickAndDrag->setText("<html><span style=\"font-size:12pt; color:#696969;\">"
                    "Click and Drag Mouse to Select Area</span></html>");
            gridLayout->addWidget(lblClickAndDrag, 1, 0, 1, 1);
            gridLayout->setColumnStretch(1, 65535);
            btnCancel2 = new QPushButton(tr("Cancel"), labelContainer);
            btnCancel2->setFocusPolicy(Qt::NoFocus);
            gridLayout->addWidget(btnCancel2, 1, 2);
            
            //page 2
            controlContainer = new QWidget(this);
            setupUi(controlContainer);
            stackedLayout = new QStackedLayout(this);
            
            stackedLayout->addWidget(labelContainer);
            stackedLayout->addWidget(controlContainer);
        }
};

#endif

