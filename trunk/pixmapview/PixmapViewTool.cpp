/* PixmapView
A widget which displays images in a scrollable container at any zoom level
Copyright (C) 2005 Wesley Crossman
Email: wesley@crossmans.net

Note that this class may not be used by programs not under the GPL without permission.
Email me if you wish to discuss the use of this class in non-GPL programs.

You can redistribute and/or modify this software under the terms of the GNU
General Public License as published by the Free Software Foundation;
either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this
program; if not, write to the Free Software Foundation, Inc., 59 Temple Place,
Suite 330, Boston, MA 02111-1307 USA */

#include "PixmapViewTool.h"
#include "PixmapView.h"

PixmapViewTool::PixmapViewTool() {
    parentView = NULL;
}

PixmapViewTool::~PixmapViewTool() {
    if (parentView) {
        parentView->setTool(NULL);
    }
}

void PixmapViewTool::setParent(PixmapView *parentView) {
    this->parentView = parentView;
}

void PixmapViewTool::mousePressEvent(QMouseEvent *e) {
}

void PixmapViewTool::mouseDoubleClickEvent(QMouseEvent *e) {
}

void PixmapViewTool::mouseReleaseEvent(QMouseEvent *e) {
}

void PixmapViewTool::mouseMoveEvent(QMouseEvent *e) {
}

void PixmapViewTool::paintEvent(QPainter &p, QPaintEvent *e) {
}
