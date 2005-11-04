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
protected:
    QPointer < CropWidget > settingsToolBar;
    QString fileName;
    QPixmap *pix;
    QObject *parent;
public:
    virtual void init(QString &fileName, lua_State *L, QObject *parent);
    virtual QImage *activate(QPixmap *pix);
    virtual QImage *activate(QImage *img);
    virtual QWidget *getSettingsToolBar(QPixmap *pix);
    virtual PixmapViewTool *getViewTool();
    virtual void reset();
    virtual int length();
    QListWidgetItem *createListEntry(QListWidget *parent);
protected slots:
    void okClicked();
    void cancelClicked();
    void selectionChanged(ImageRect);
};

class CropWidget : public QWidget, public Ui::CropWidget {
    public:
        QHBoxLayout *hboxLayout;
        QStackedLayout *stackedLayout;
        QLabel *lblClickAndDrag;
        QPushButton *btnOk, *btnCancel;
        QWidget *controls;
        CropWidget(QWidget *parent) : QWidget(parent) {
            QPalette p;
            p.setColor(QPalette::Background, Qt::red);
            setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            controls = new QWidget(this);
            setupUi(controls);
            hboxLayout = new QHBoxLayout(this);
            hboxLayout->setMargin(4);
            stackedLayout = new QStackedLayout();
            stackedLayout->setMargin(0);
            lblClickAndDrag = new QLabel(this);
            lblClickAndDrag->setText("<html><span style=\"font-size:12pt; font-style:italic; color:#696969;\">"
                    "Click and Drag Mouse to Select Area</span></html>");
            stackedLayout->addWidget(lblClickAndDrag);
            stackedLayout->addWidget(controls);
            hboxLayout->addLayout(stackedLayout);
            hboxLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
            btnOk = new QPushButton(tr("OK"), this);
            hboxLayout->addWidget(btnOk);
            btnCancel = new QPushButton(tr("Cancel"), this);
            btnCancel->setFocusPolicy(Qt::NoFocus);
            hboxLayout->addWidget(btnCancel);
        }
};

#endif

