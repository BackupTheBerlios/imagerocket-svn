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

#include "RocketSaveSettingsTool.h"
#include "RocketInfoTool.h"
#include "interfaces.h"

SaveSettingsWidget::SaveSettingsWidget() {
    setupUi(this);
}

bool RocketSaveSettingsTool::previewCheckedByDefault = true;

RocketSaveSettingsTool::RocketSaveSettingsTool(QObject *parent) : QObject(parent) {
    updateTimer.setSingleShot(true);
    connect(&updateTimer, SIGNAL(timeout()), SLOT(updatePreview()));
}

QWidget *RocketSaveSettingsTool::getSettingsToolBar(RocketImage *img) {
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
    connect(widget->cmbType, SIGNAL(activated(int)), SLOT(setSaveFormat(int)));
    widget->chkProgressiveLoading->setChecked(img->isSaveProgressive());
    connect(widget->chkProgressiveLoading, SIGNAL(toggled(bool)), img, SLOT(setSaveProgressive(bool)));
    connect(widget->chkShowPreview, SIGNAL(toggled(bool)), SLOT(previewToggled(bool)));
    connect(widget->chkShowPreview, SIGNAL(toggled(bool)), &updateTimer, SLOT(start()));
    widget->cmbType->setCurrentIndex(img->getSaveFormat());
    widget->lblSize->setText(tr("<html><span style=\" font-style:italic;\">Calculating...</span></html>"));
    setSaveFormat(img->getSaveFormat());
    return widget;
}

void RocketSaveSettingsTool::setSaveFormat(int format) {
    widget->sldQuality->setEnabled(format == 0);
    widget->chkProgressiveLoading->setEnabled(format == 0);
    widget->lblQuality->setEnabled(format == 0);
    widget->lblQualityValue->setEnabled(format == 0);
    img->setSaveFormat(format);
    updateTimer.start(250);
}

void RocketSaveSettingsTool::updatePreview() {
    if (widget && previewCheckedByDefault) {
        QBuffer buffer;
        img->generateSavedFileInMemory(buffer);
        QImage i(QImage::fromData(buffer.data())); 
        widget->lblSize->setText(
                tr("<html>Image Size: %1</html>")
                .arg(RocketInfoTool::getSizeFormattedForDisplay(img->getSizeOfFileWhenSaved())));
        UpdatePreviewToolEvent *event = new UpdatePreviewToolEvent;
        event->pixmap = new QPixmap(QPixmap::fromImage(i));
        QCoreApplication::sendEvent(parent(), event);
    } else if (widget) {
        widget->lblSize->setText(tr("<html><i>Check \"Preview Toggled\" to see size</i></html>"));
        sendPreviewOff();
    }
    //needed since getSettingsToolBar starts it with a short interval - WJC
    updateTimer.setInterval(750);
}

void RocketSaveSettingsTool::sendPreviewOff() {
    UpdatePreviewToolEvent *event = new UpdatePreviewToolEvent;
    QCoreApplication::sendEvent(parent(), event);
}

void RocketSaveSettingsTool::previewToggled(bool value) {
    previewCheckedByDefault = value;
    updatePreview();
}
