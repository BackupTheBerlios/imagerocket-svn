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

#include "RocketToolBox.h"
#include <algorithm>

RocketToolBox::RocketToolBox(QWidget *parent) : QListWidget(parent) {
    setFocusPolicy(Qt::NoFocus);
    setSpacing(1);
    QFont f(font());
    f.setPointSize(f.pointSize()+2);
    setFont(f);
}

void RocketToolBox::updateMinimumSize() {
    int fitWidth = 0;
    for (int a=0;a<count();++a) {
        fitWidth = std::max(rectForIndex(indexFromItem(item(a))).width(), fitWidth);
    }
    setMinimumWidth(fitWidth + 5);
}

QSize RocketToolBox::sizeHint() const {
    return QSize(30, 50);
}

void RocketToolBox::addSeparator() {
    QFrame *line = new QFrame(this);
    line->setEnabled(false);
    line->setFrameShape(QFrame::HLine);
    addItem("");
    item(count()-1)->setSizeHint(QSize(1, 4));
    setItemWidget(item(count()-1), line);
}
