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

#include "ResizeViewTool.h"
#include <algorithm>

ResizeViewTool::ResizeViewTool() {
}

void ResizeViewTool::setParent(PixmapView *parent) {
    PixmapViewTool::setParent(parent);
}

void ResizeViewTool::paintEvent(QPainter &p, QPaintEvent *e) {
    p.save();
    p.setBrush(QBrush(Qt::white, Qt::BDiagPattern));
    p.setPen(Qt::white);
    QSize viewport(parentView->viewport()->size());
    QRect rect(viewport.width()/2-imageSize.width()/2,
            viewport.height()/2-imageSize.height()/2,
            imageSize.width(), imageSize.height());
    p.drawRect(rect);
    p.setPen(Qt::black);
    QString text(tr("This box shows how large the resulting image will be."));
    QRect textRect(p.boundingRect(rect, Qt::AlignHCenter|Qt::AlignVCenter|Qt::TextWordWrap, text));
    p.fillRect(rect.intersect(textRect), QColor(255, 255, 255, 128));
    p.drawText(rect, Qt::AlignHCenter|Qt::AlignVCenter|Qt::TextWordWrap, text);
    p.restore();
}

void ResizeViewTool::mouseMoveEvent(QMouseEvent *e) {
}

void ResizeViewTool::mousePressEvent(QMouseEvent *e) {
}

void ResizeViewTool::mouseDoubleClickEvent(QMouseEvent *e) {
}

void ResizeViewTool::setSize(QSize imageSize) {
    this->imageSize = imageSize;
    if (parentView) {
        parentView->viewport()->update();
    }
}
