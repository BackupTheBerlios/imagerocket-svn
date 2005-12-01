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
#include "interfaces.h"

SaveSettingsWidget::SaveSettingsWidget() {
    setupUi(this);
}

bool SaveSettingsTool::previewCheckedByDefault = true;

SaveSettingsTool::SaveSettingsTool(QObject *parent) : QObject(parent) {
    updateTimer.setInterval(750);
    updateTimer.setSingleShot(true);
    connect(&updateTimer, SIGNAL(timeout()), SLOT(updatePreview()));
}

QWidget *SaveSettingsTool::getSettingsToolBar(RocketImage *img) {
    this->img = img;
    widget = new SaveSettingsWidget();
    connect(new QShortcut(QKeySequence("Esc"), widget),
            SIGNAL(activated()), widget, SLOT(deleteLater()));
    connect(new QShortcut(QKeySequence("Enter"), widget),
            SIGNAL(activated()), widget, SLOT(deleteLater()));
    connect(new QShortcut(QKeySequence("Return"), widget),
            SIGNAL(activated()), widget, SLOT(deleteLater()));
    connect(widget, SIGNAL(destroyed()), SLOT(sendPreviewOff()));
    widget->chkShowPreview->setChecked(previewCheckedByDefault);
    widget->sldQuality->setValue(img->getSaveQuality());
    connect(widget->sldQuality, SIGNAL(valueChanged(int)), img, SLOT(setSaveQuality(int)));
    connect(widget->sldQuality, SIGNAL(valueChanged(int)), &updateTimer, SLOT(start()));
    widget->cmbType->setCurrentIndex(img->getSaveFormat());
    connect(widget->cmbType, SIGNAL(activated(int)), img, SLOT(setSaveFormat(int)));
    connect(widget->cmbType, SIGNAL(activated(int)), &updateTimer, SLOT(start()));
    widget->chkProgressiveLoading->setChecked(img->isSaveProgressive());
    connect(widget->chkProgressiveLoading, SIGNAL(toggled(bool)), img, SLOT(setSaveProgressive(bool)));
    connect(widget->chkShowPreview, SIGNAL(toggled(bool)), SLOT(previewToggled(bool)));
    connect(widget->chkShowPreview, SIGNAL(toggled(bool)), &updateTimer, SLOT(start()));
    updatePreview();
    return widget;
}

void SaveSettingsTool::updatePreview() {
    if (widget && previewCheckedByDefault) {
        QBuffer buffer;
        img->generateSavedFileInMemory(buffer);
        QImage i(QImage::fromData(buffer.data()));
        int size = buffer.size()/1024;
        int adjusted = int(size * 1.1) + 10;
        widget->lblSize->setText(
                tr("<html>Image Size: <i>%L1k</i> &#8213; "
                "Download Time: Dialup <i>%L2 seconds</i>, "
                "High Speed <i>%L3 seconds</i></html>", "8213 is a long hyphen")
                .arg(size).arg(int(adjusted/6)).arg(int(adjusted/64)));
        UpdatePreviewToolEvent *event = new UpdatePreviewToolEvent;
        event->pixmap = new QPixmap(QPixmap::fromImage(i));
        QCoreApplication::sendEvent(parent(), event);
    } else if (widget) {
        widget->lblSize->setText(tr("<html><i>Check \"Preview Toggled\" to see size</i></html>"));
        sendPreviewOff();
    }
}

void SaveSettingsTool::sendPreviewOff() {
    UpdatePreviewToolEvent *event = new UpdatePreviewToolEvent;
    QCoreApplication::sendEvent(parent(), event);
}

void SaveSettingsTool::previewToggled(bool value) {
    previewCheckedByDefault = value;
    updatePreview();
}
