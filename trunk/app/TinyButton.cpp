/* TinyButton
This is a button unconstrained by QApplication::globalStrut, so it's as small as its text.
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

//Trolltech code in this class is used under the GPL

#include "TinyButton.h"
#include <QStyleOptionButton>
#include <QFontMetrics>

/*!
  \class RocketWindow
  \short This is a button which is always as small as its text, unlike its parent class.
   This does not support icons, and it will behave as a normal button if they are used.
   I expect there are also some other things it does not support as well. This is a hack.
*/

TinyButton::TinyButton(QWidget *parent) : QPushButton(parent) {
    widthForcedToHeight = false;
}

QStyleOptionButton TinyButton::getButtonStyleOption() const {
    //This function borrows from Qt's qpushbutton.cpp.
    //It's a shame QPushButton's getStyleOption is private. This function
    //would not have been necessary if this weren't the case. This function
    //is not a complete copy, so it will not properly set all the features
    //the parent class supports. - WJC
    QStyleOptionButton opt;
    opt.init(this);
    opt.features = QStyleOptionButton::None;
    if (isFlat())
        opt.features |= QStyleOptionButton::Flat;
    if (autoDefault() || isDefault())
        opt.features |= QStyleOptionButton::AutoDefaultButton;
    if (isDefault())
        opt.features |= QStyleOptionButton::DefaultButton;
    if (isDown())
        opt.state |= QStyle::State_Sunken;
    if (isChecked())
        opt.state |= QStyle::State_On;
    if (!isFlat() && !isDown())
        opt.state |= QStyle::State_Raised;
    opt.text = text();
    opt.icon = icon();
    opt.iconSize = iconSize();
    return opt;
}

QSize TinyButton::sizeHint() const {
    //This function borrows from Qt's qpushbutton.cpp.
    if (!icon().isNull()) {
        return QPushButton::sizeHint();
    }
    ensurePolished();
    int w = 0;
    QStyleOptionButton opt = getButtonStyleOption();
    QString s(text());
    if (s.isEmpty()) {
        s = QString::fromLatin1("X");
    }
    QFontMetrics fm = fontMetrics();
    QSize sz = fm.size(Qt::TextShowMnemonic, s);
    w += sz.width();
    w += style()->pixelMetric(QStyle::PM_DefaultFrameWidth, &opt, this);
    w += style()->pixelMetric(QStyle::PM_ButtonMargin, &opt, this);
    int h = QPushButton::sizeHint().height();
    if (widthForcedToHeight) {
        return QSize(qMax(h, w), h);
    } else {
        return QSize(w, h);
    }
}
