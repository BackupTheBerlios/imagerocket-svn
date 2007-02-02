/* ImageRocket
An image-editing program written for editing speed and ease of use.
Copyright (C) 2005-2007 Wesley Crossman
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

#include "RocketInfoTool.h"

RocketInfoWidget::RocketInfoWidget() {
    setupUi(this);
}

void RocketInfoWidget::mouseReleaseEvent(QMouseEvent *) {
    deleteLater();
}

QString RocketInfoTool::getSizeFormattedForDisplay(int bits) {
    int bytes = bits/1024;
    int adjusted = int(bytes * 1.1) + 10;
    QString desc =
            tr("%L1k -- Download Time: Dialup %L2 seconds, High Speed %L3 seconds")
            .arg(bytes).arg(int(adjusted/6)).arg(int(adjusted/64));
    return desc;
}

RocketInfoTool::RocketInfoTool(QObject *parent) : QObject(parent) {
}

QWidget *RocketInfoTool::getInfoToolBar(RocketImage *img) {
    this->img = img;
    widget = new RocketInfoWidget();
    widget->lblFileName->setText(img->getShortFileName());
    QFont font = widget->lblFileName->font();
    font.setBold(true);
    widget->lblFileName->setFont(font);
    QFile file(img->getFileName());
    QFileInfo info(file);
    QDateTime lm = info.lastModified();
    QString date = lm.toString(QLocale::system().dateFormat());
    QString time = lm.toString(QLocale::system().timeFormat(QLocale::ShortFormat));
    widget->lblLastModified->setText(tr("%1 - %2", "date - time").arg(date).arg(time));
    widget->lblCurrentFileSize->setText(getSizeFormattedForDisplay(info.size()));
    widget->lblSavedFileSize->setText(getSizeFormattedForDisplay(img->getSizeOfFileWhenSaved()));
    return widget;
}
