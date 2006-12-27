/* ImageRocket
An image-editing program written for editing speed and ease of use.
Copyright (C) 2005-2006 Wesley Crossman
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


#include "RocketImage.h"
#include "RocketOptionsDialog.h"

RocketOptionsDialog::RocketOptionsDialog(QWidget *parent) : QDialog(parent) {
    setupUi(this);
    
    positionGroup.addButton(radPosition1, 1);
    positionGroup.addButton(radPosition2, 2);
    positionGroup.addButton(radPosition3, 3);
    positionGroup.addButton(radPosition4, 4);
    positionGroup.addButton(radPosition5, 5);
    positionGroup.addButton(radPosition6, 6);
    positionGroup.addButton(radPosition7, 7);
    positionGroup.addButton(radPosition8, 8);
    positionGroup.addButton(radPosition9, 9);
    connect(&positionGroup, SIGNAL(buttonClicked(int)), SLOT(positionClicked(int)));
    
    QSettings settings;
    settings.beginGroup("thumbnail");
    chkAlwaysMakeThumbnails->setChecked(settings.value("alwaysMakeThumbnails").toBool());
    settings.endGroup();
    settings.beginGroup("ui");
    chkAskBeforeDeleting->setChecked(settings.value("askBeforeDeleting").toBool());
    chkUseFading->setChecked(settings.value("useFading").toBool());
    chkShowRollover->setChecked(settings.value("showRollover").toBool());
    chkCenterOnSelection->setChecked(settings.value("centerOnSelection").toBool());
    settings.endGroup();
    spnRamForImages->setValue(settings.value("image/ramForImages").toInt());
    chkCheckForUpdates->setChecked(settings.value("program/checkForUpdates").toBool());
    settings.beginGroup("watermark");
    watermarkFont = settings.value("font").value< QFont >();
    QFont textFont = watermarkFont;
    textFont.setPointSize(txtWatermarkText->font().pointSize());
    txtWatermarkText->setFont(textFont); //disregard point size
    spnMargin->setValue(settings.value("margin").toInt());
    txtWatermarkText->setPlainText(settings.value("text").toString());
    if (settings.value("selected").toString() == "text") {
        radWatermarkText->setChecked(true);
    } else {
        radImage->setChecked(true);
    }
    if (QFile(settings.value("image").toString()).exists()) {
        linImage->setText(settings.value("image").toString());
    }
    chkAddWatermark->setChecked(settings.value("on").toBool());
    positionGroup.button(settings.value("position").toInt())->setChecked(true);
    watermarkColor = settings.value("color").value< QColor >();
    QColor buttonColor = watermarkColor;
    buttonColor.setAlpha(255);
    setColorButtonColor(buttonColor); //disregard alpha
    sldOpacity->setValue(watermarkColor.alpha());
    settings.endGroup();
    
    connect(btnColor, SIGNAL(clicked()), SLOT(colorClicked()));
    connect(sldOpacity, SIGNAL(valueChanged(int)), SLOT(opacityChanged()));
    connect(btnFont, SIGNAL(clicked()), SLOT(fontClicked()));
    connect(btnImageSelect, SIGNAL(clicked()), SLOT(imageSelectClicked()));
    connect(btnPreview, SIGNAL(clicked()), SLOT(previewClicked()));
}

void RocketOptionsDialog::accept() {
    QSettings settings;
    settings.beginGroup("thumbnail");
    settings.setValue("alwaysMakeThumbnails", chkAlwaysMakeThumbnails->isChecked());
    settings.endGroup();
    settings.beginGroup("ui");
    settings.setValue("askBeforeDeleting", chkAskBeforeDeleting->isChecked());
    settings.setValue("useFading", chkUseFading->isChecked());
    settings.setValue("showRollover", chkShowRollover->isChecked());
    settings.setValue("centerOnSelection", chkCenterOnSelection->isChecked());
    settings.endGroup();
    settings.setValue("image/ramForImages", spnRamForImages->value());
    settings.setValue("program/checkForUpdates", chkCheckForUpdates->isChecked());
    settings.beginGroup("watermark");
    settings.setValue("on", chkAddWatermark->isChecked());
    settings.setValue("position", positionGroup.id(positionGroup.checkedButton()));
    settings.setValue("color", watermarkColor);
    settings.setValue("font", watermarkFont);
    settings.setValue("margin", spnMargin->value());
    settings.setValue("text", txtWatermarkText->toPlainText());
    if (radWatermarkText->isChecked()) {
        settings.setValue("selected", "text");
    } else {
        settings.setValue("selected", "image");
    }
    settings.setValue("image", linImage->text());
    settings.endGroup();
    QDialog::accept();
}

void RocketOptionsDialog::reject() {
    QDialog::reject();
}

void RocketOptionsDialog::positionClicked(int index) {
    spnMargin->setEnabled(index != 5);
}

void RocketOptionsDialog::colorClicked() {
    QColor defaultColor = watermarkColor;
    defaultColor.setAlpha(255);
    QColor color = QColorDialog::getColor(defaultColor, this);
    if (color.isValid()) {
        setColorButtonColor(color);
        color.setAlpha(sldOpacity->value());
        watermarkColor = color;
        radWatermarkText->setChecked(true);
    }
}

void RocketOptionsDialog::opacityChanged() {
    watermarkColor.setAlpha(sldOpacity->value());
}

void RocketOptionsDialog::setColorButtonColor(QColor color) {
    QImage image(16, 16, QImage::Format_ARGB32_Premultiplied);
    image.fill(qRgb(color.red(), color.green(), color.blue()));
    QPainter p(&image);
    p.setPen(Qt::black);
    p.drawRect(0, 0, image.width()-1, image.height()-1);
    p.end();
    btnColor->setIcon(QPixmap::fromImage(image));
}

void RocketOptionsDialog::fontClicked() {
    bool ok = false;
    QFont font = QFontDialog::getFont(&ok, watermarkFont, this);
    if (ok) {
        watermarkFont = font;
        font.setPointSize(txtWatermarkText->font().pointSize());
        txtWatermarkText->setFont(font);
        radWatermarkText->setChecked(true);
    }
}

void RocketOptionsDialog::imageSelectClicked() {
    static QString location = QString(QDir::homePath());
    QStringList imageNameFilters;
    foreach (QByteArray format, QImageReader::supportedImageFormats()) {
        imageNameFilters.append(QString("*.") + QString(format).toLower());
    }
    QString filter = tr("Images (%1)").arg(imageNameFilters.join(" "));
    QString file = QFileDialog::getOpenFileName(this, tr("Select Image"), location, filter);
    if (!file.isNull()) {
        location = QFileInfo(file).dir().absolutePath();
        linImage->setText(file);
        radImage->setChecked(true);
    }
}

void RocketOptionsDialog::previewClicked() {
    QImage image(":/pixmaps/preview.jpg");
    QVariant contents = radWatermarkText->isChecked()
            ? QVariant(txtWatermarkText->toPlainText()) : QVariant(QImage(linImage->text()));
    RocketImage::renderWatermark(&image, contents, watermarkFont,
            watermarkColor, spnMargin->value(), positionGroup.checkedId());
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle(tr("Watermark Preview"));
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setMargin(0);
    layout->setSpacing(0);
    QLabel *label = new QLabel(dialog);
    label->setPixmap(QPixmap::fromImage(image));
    layout->addWidget(label);
    QPushButton *close = new QPushButton(tr("Close"), dialog);
    layout->addWidget(close);
    connect(close, SIGNAL(clicked()), dialog, SLOT(close()));
    dialog->setMaximumSize(size());
    dialog->exec();
}
