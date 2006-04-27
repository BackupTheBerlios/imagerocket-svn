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

#ifndef ROCKET_OPTIONS_DIALOG_H
#define ROCKET_OPTIONS_DIALOG_H

#include "ui_optionsdialog.h"
#include <QtGui>

class RocketOptionsDialog : public QDialog, private Ui::RocketOptionsDialog {
Q_OBJECT
public:
    RocketOptionsDialog(QWidget *parent);
protected:
    QButtonGroup positionGroup;
    QColor watermarkColor;
    QFont watermarkFont;
    void setColorButtonColor(QColor color);
protected slots:
    void okClicked();
    void cancelClicked();
    void positionClicked(int index);
    void colorClicked();
    void opacityChanged();
    void fontClicked();
    void previewClicked();
};

#endif
