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

#include "CropViewTool.h"
#include <algorithm>

#define PADDING 7

CropViewTool::CropViewTool() {
    selection = ImageRect();
    left = right = top = bottom = move = false;
}

void CropViewTool::setParent(PixmapView *parent) {
    PixmapViewTool::setParent(parent);
}

bool CropViewTool::isOnLeft(const QRect &r, const QPoint p) {
    return QRect(r.left()-PADDING, r.top(), PADDING, r.height()).contains(p);
}

bool CropViewTool::isOnRight(const QRect &r, const QPoint p) {
    return QRect(r.right(), r.top(), PADDING, r.height()).contains(p);
}

bool CropViewTool::isOnTop(const QRect &r, const QPoint p) {
    return QRect(r.left(), r.top()-PADDING, r.width(), PADDING).contains(p);
}

bool CropViewTool::isOnBottom(const QRect &r, const QPoint p) {
    return QRect(r.left(), r.bottom(), r.width(), PADDING).contains(p);
}

bool CropViewTool::isOnVertical(const QRect &r, const QPoint p) {
    return isOnLeft(r, p) || isOnRight(r, p);
}

bool CropViewTool::isOnHorizontal(const QRect &r, const QPoint p) {
    return isOnTop(r, p) || isOnBottom(r, p);
}

bool CropViewTool::isOnNw(const QRect &r, const QPoint p) {
    return QRect(r.left()-PADDING, r.top()-PADDING, PADDING, PADDING).contains(p);
}

bool CropViewTool::isOnNe(const QRect &r, const QPoint p) {
    return QRect(r.right(), r.top()-PADDING, PADDING, PADDING).contains(p);
}

bool CropViewTool::isOnSw(const QRect &r, const QPoint p) {
    return QRect(r.left()-PADDING, r.bottom(), PADDING, PADDING).contains(p);
}

bool CropViewTool::isOnSe(const QRect &r, const QPoint p) {
    return QRect(r.right(), r.bottom(), PADDING, PADDING).contains(p);
}

bool CropViewTool::isOnNeSw(const QRect &r, const QPoint p) {
    return isOnNe(r, p) || isOnSw(r, p);
}

bool CropViewTool::isOnNwSe(const QRect &r, const QPoint p) {
    return isOnNw(r, p) || isOnSe(r, p);
}

void CropViewTool::setSelection(ImageRect ir) {
    if (ir != selection) {
        QRect r(ir.unite(selection));
        parentView->viewport()->update(
                parentView->toScreenRect(ImageRect::fromRect(r)).adjusted(-4, -4, 4, 4));
        selection = ir;
        emit selectionChanged(selection);
    }
}

void CropViewTool::paintEvent(QPainter &p, QPaintEvent *e) {
    if (selection.isNull()) return;
    ScreenPoint drawStart(parentView->toScreenPoint(PhysicalPoint(0, 0)));
    QSize drawSize(parentView->getImageSize() * parentView->getZoom());
    QRect darkRect(e->rect().intersect(QRect(drawStart, drawSize)));
    QRegion darkRegion(darkRect);
    ScreenRect selectedRect(parentView->toScreenRect(selection));
    darkRegion -= selectedRect;
    foreach (QRect r, darkRegion.rects()) {
        p.fillRect(r, QBrush(QColor(0, 0, 0, 128)));
    }
    p.setPen(Qt::blue);
    p.drawRect(selectedRect.adjusted(-1, -1, 1, 1));
    p.setPen(Qt::white);
    p.drawRect(selectedRect.adjusted(-2, -2, 2, 2));
}

void CropViewTool::mouseMoveEvent(QMouseEvent *e) {
    if (selection.isNull()) return;
    if (e->buttons() & Qt::LeftButton) {
        QPoint p(parentView->toImagePoint(ScreenPoint(e->pos().x(), e->pos().y())) - dragStart);
        ImagePoint ip(parentView->toImagePoint(PhysicalPoint(p.x(), p.y())));
        bool change = false;
        ImageRect oldSelection(selection);
        if (left) {
            selection.setLeft(std::min(
                    std::max(preDragSelection.left()+p.x(), 0), selection.right()-1));
            change = true;
        }
        if (right) {
            selection.setRight(std::max(std::min(preDragSelection.right()+p.x(),
                    parentView->getImageSize().width()-1), selection.left()+1));
            change = true;
        }
        if (top) {
            selection.setTop(std::min(
                    std::max(preDragSelection.top()+p.y(), 0), selection.bottom()-1));
            change = true;
        }
        if (bottom) {
            selection.setBottom(std::max(std::min(preDragSelection.bottom()+p.y(),
                    parentView->getImageSize().height()-1), selection.top()+1));
            change = true;
        }
        if (move) {
            int x = std::min(std::max(0, preDragSelection.left()+p.x()),
                    parentView->getImageSize().width()-selection.width());
            int y = std::min(std::max(0, preDragSelection.top()+p.y()),
                    parentView->getImageSize().height()-selection.height());
            selection.moveTopLeft(QPoint(x,y));
            change = true;
        }
        if (change) {
            emit selectionChanged(selection);
            QRect r(oldSelection.unite(selection));
            parentView->viewport()->update(
                    parentView->toScreenRect(ImageRect::fromRect(r)).adjusted(-4, -4, 4, 4));
        }
    } else {
        ScreenRect s(parentView->toScreenRect(selection));
        if (isOnNeSw(s, e->pos())) {
            parentView->viewport()->setCursor(Qt::SizeBDiagCursor);
            return;
        } else if (isOnNwSe(s, e->pos())) {
            parentView->viewport()->setCursor(Qt::SizeFDiagCursor);
            return;
        }
        if (isOnVertical(s, e->pos())) {
            parentView->viewport()->setCursor(Qt::SizeHorCursor);
        } else if (isOnHorizontal(s, e->pos())) {
            parentView->viewport()->setCursor(Qt::SizeVerCursor);
        //} else if (s.contains(e->pos())) {
        //    parentView->viewport()->setCursor(QCursor(Qt::SizeAllCursor));
        } else {
            parentView->viewport()->unsetCursor();
        }
    }
}

void CropViewTool::mousePressEvent(QMouseEvent *e) {
    if (e->button() != Qt::LeftButton) return;
    QRect sRect(parentView->toScreenRect(selection));
    ImagePoint ip(parentView->toImagePoint(ScreenPoint::fromPoint(e->pos())));
    ImageRect image(0, 0, parentView->getImageSize().width(), parentView->getImageSize().height());
    left = right = top = bottom = move = false;
    if (isOnNw(sRect, e->pos())) {
        dragStart = ip;
        preDragSelection = selection;
        top = left = true;
    } else if (isOnNe(sRect, e->pos())) {
        dragStart = ip;
        preDragSelection = selection;
        top = right = true;
    } else if (isOnSw(sRect, e->pos())) {
        dragStart = ip;
        preDragSelection = selection;
        bottom = left = true;
    } else if (isOnSe(sRect, e->pos())) {
        dragStart = ip;
        preDragSelection = selection;
        bottom = right = true;
    } else if (isOnLeft(sRect, e->pos())) {
        dragStart = ip;
        preDragSelection = selection;
        left = true;
    } else if (isOnRight(sRect, e->pos())) {
        dragStart = ip;
        preDragSelection = selection;
        right = true;
    } else if (isOnTop(sRect, e->pos())) {
        dragStart = ip;
        preDragSelection = selection;
        top = true;
    } else if (isOnBottom(sRect, e->pos())) {
        dragStart = ip;
        preDragSelection = selection;
        bottom = true;
    } else if (sRect.contains(e->pos(), true)) {
        dragStart = ip;
        preDragSelection = selection;
        move = true;
    } else if (image.contains(ip)) {
        selection.setRect(ip.x(), ip.y(), 1, 1);
        dragStart = ip;
        preDragSelection = selection;
        right = bottom = true;
        emit selectionChanged(selection);
        parentView->viewport()->update();
    }
}

void CropViewTool::mouseDoubleClickEvent(QMouseEvent *e) {
    emit selected();
}
