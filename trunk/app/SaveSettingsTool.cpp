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

#include "SaveSettingsTool.h"

SaveSettingsWidget::SaveSettingsWidget() {
    setupUi(this);
}

SaveSettingsTool::SaveSettingsTool(QObject *parent) : QObject(parent) {
    previewEnabled = true;
}

QWidget *SaveSettingsTool::getSettingsToolBar(RocketImage *img) {
    this->img = img;
    widget = new SaveSettingsWidget();
    widget->chkShowPreview->setChecked(previewEnabled);
    widget->sldQuality->setValue(img->getSaveQuality());
    connect(widget->sldQuality, SIGNAL(valueChanged(int)), img, SLOT(setSaveQuality(int)));
    widget->cmbType->setCurrentIndex(img->getSaveFormat());
    connect(widget->cmbType, SIGNAL(activated(int)), img, SLOT(setSaveFormat(int)));
    widget->chkProgressiveLoading->setChecked(img->isSaveProgressive());
    connect(widget->chkProgressiveLoading, SIGNAL(toggled(bool)), img, SLOT(setSaveProgressive(bool)));
    connect(widget->chkShowPreview, SIGNAL(toggled(bool)), SLOT(previewToggled(bool)));
    return widget;
}

void SaveSettingsTool::previewToggled(bool value) {
    previewEnabled = value;
}
