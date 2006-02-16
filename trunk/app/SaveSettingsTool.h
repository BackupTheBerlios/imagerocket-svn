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

#ifndef SAVE_SETTINGS_TOOL_H
#define SAVE_SETTINGS_TOOL_H

#include <QWidget>
#include "RocketImage.h"
#include "ui_savesettingstool.h"

class SaveSettingsWidget : public QWidget, public Ui::SaveSettingsTool {
Q_OBJECT
public:
    SaveSettingsWidget();
};

class SaveSettingsTool : public QObject {
Q_OBJECT
public:
    SaveSettingsTool(QObject *parent = NULL);
    QWidget *getSettingsToolBar(RocketImage *img);
protected:
    RocketImage *img;
    QPointer < SaveSettingsWidget > widget;
    static bool previewCheckedByDefault;
    QTimer updateTimer;
protected slots:
    void updatePreview();
    void previewToggled(bool);
    void sendPreviewOff();
    void setSaveFormat(int);
};

#endif
