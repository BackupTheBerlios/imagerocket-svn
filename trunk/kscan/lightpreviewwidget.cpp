/* LightPreviewWidget
   A preview widget intending for showing preview scans
   Copyright (C) 2006 Wesley Crossman
   Email: wesley@crossmans.net

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "lightpreviewwidget.h"
#include <algorithm>

#define PADDING 7

LightPreviewWidget::LightPreviewWidget(QWidget *parent) : QWidget(parent) {
    left = right = top = bottom = move = false;
    setMouseTracking(true);
    selectAll();
}

void LightPreviewWidget::setPixmap(const QPixmap &pix) {
    QPixmap oldPixmap = pixmap;
    pixmap = pix;
    setMinimumSize(sizeHint());
    updateGeometry();
    if (oldPixmap.size() != pix.size()) selectAll();
}

QSize LightPreviewWidget::sizeHint() {
    QSize size = (pixmap.isNull() ? QSize(0, 0) : pixmap.size()) + QSize(8, 8);
    return size;
}

QPixmap LightPreviewWidget::getPixmap() {
    return pixmap;
}

void LightPreviewWidget::selectAll() {
    selection = QRect(0, 0, pixmap.width(), pixmap.height());
    emit selectionChanged(selection);
    update();
}

bool LightPreviewWidget::isOnLeft(const QRect &r, const QPoint p) {
    return QRect(r.left()-PADDING, r.top(), PADDING, r.height()).contains(p);
}

bool LightPreviewWidget::isOnRight(const QRect &r, const QPoint p) {
    return QRect(r.right(), r.top(), PADDING, r.height()).contains(p);
}

bool LightPreviewWidget::isOnTop(const QRect &r, const QPoint p) {
    return QRect(r.left(), r.top()-PADDING, r.width(), PADDING).contains(p);
}

bool LightPreviewWidget::isOnBottom(const QRect &r, const QPoint p) {
    return QRect(r.left(), r.bottom(), r.width(), PADDING).contains(p);
}

bool LightPreviewWidget::isOnVertical(const QRect &r, const QPoint p) {
    return isOnLeft(r, p) || isOnRight(r, p);
}

bool LightPreviewWidget::isOnHorizontal(const QRect &r, const QPoint p) {
    return isOnTop(r, p) || isOnBottom(r, p);
}

bool LightPreviewWidget::isOnNw(const QRect &r, const QPoint p) {
    return QRect(r.left()-PADDING, r.top()-PADDING, PADDING, PADDING).contains(p);
}

bool LightPreviewWidget::isOnNe(const QRect &r, const QPoint p) {
    return QRect(r.right(), r.top()-PADDING, PADDING, PADDING).contains(p);
}

bool LightPreviewWidget::isOnSw(const QRect &r, const QPoint p) {
    return QRect(r.left()-PADDING, r.bottom(), PADDING, PADDING).contains(p);
}

bool LightPreviewWidget::isOnSe(const QRect &r, const QPoint p) {
    return QRect(r.right(), r.bottom(), PADDING, PADDING).contains(p);
}

bool LightPreviewWidget::isOnNeSw(const QRect &r, const QPoint p) {
    return isOnNe(r, p) || isOnSw(r, p);
}

bool LightPreviewWidget::isOnNwSe(const QRect &r, const QPoint p) {
    return isOnNw(r, p) || isOnSe(r, p);
}

void LightPreviewWidget::setSelection(QRect rect) {
    #if 0
    if (ir != selection) {
        QRect r(ir.unite(selection));
        update(
                parentView->toScreenRect(ImageRect::fromRect(r)).adjusted(-4, -4, 4, 4));
        selection = ir;
        emit selectionChanged(selection);
    }
    #endif
}

void LightPreviewWidget::paintEvent(QPaintEvent *e) {
    if (pixmap.isNull()) return;
    QPainter p(this);
    p.setClipRect(e->rect());
    QPoint drawStart(toScreenPoint(QPoint(0, 0)));
    p.drawPixmap(drawStart, pixmap);
    p.setPen(Qt::black);
    p.drawRect(QRect(drawStart, pixmap.size()).adjusted(-1, -1, 0, 0));
    if (!selection.isNull()) {
        QRect sRect(toScreenRect(selection));
        QRect darkRect(e->rect().intersect(QRect(drawStart, pixmap.size())));
        QRegion darkRegion(darkRect);
        darkRegion -= sRect;
        foreach (QRect r, darkRegion.rects()) {
            p.fillRect(r, QBrush(QColor(0, 0, 0, 128)));
        }
        p.setPen(Qt::blue);
        p.drawRect(sRect.adjusted(-1, -1, 0, 0));
        p.setPen(Qt::white);
        p.drawRect(sRect.adjusted(-2, -2, 1, 1));
    }
}

void LightPreviewWidget::mouseMoveEvent(QMouseEvent *e) {
    if (selection.isNull()) return;
    if (e->buttons() & Qt::LeftButton) {
        QPoint p(toPhysicalPoint(QPoint(e->pos().x(), e->pos().y())) - dragStart);
        bool change = false;
        QRect oldSelection(selection);
        if (left) {
            selection.setLeft(std::min(
                    std::max(preDragSelection.left()+p.x(), 0), selection.right()-1));
            change = true;
        }
        if (right) {
            selection.setRight(std::max(std::min(preDragSelection.right()+p.x(),
                    pixmap.width()-1), selection.left()+1));
            change = true;
        }
        if (top) {
            selection.setTop(std::min(
                    std::max(preDragSelection.top()+p.y(), 0), selection.bottom()-1));
            change = true;
        }
        if (bottom) {
            selection.setBottom(std::max(std::min(preDragSelection.bottom()+p.y(),
                    pixmap.height()-1), selection.top()+1));
            change = true;
        }
        if (move) {
            int x = std::min(std::max(0, preDragSelection.left()+p.x()),
                    pixmap.width()-selection.width());
            int y = std::min(std::max(0, preDragSelection.top()+p.y()),
                    pixmap.height()-selection.height());
            selection.moveTopLeft(QPoint(x,y));
            change = true;
        }
        if (change) {
            emit selectionChanged(selection);
            QRect r(oldSelection.unite(selection));
            update();
        }
    } else {
        QRect s(toScreenRect(selection));
        if (isOnNeSw(s, e->pos())) {
            setCursor(Qt::SizeBDiagCursor);
            return;
        } else if (isOnNwSe(s, e->pos())) {
            setCursor(Qt::SizeFDiagCursor);
            return;
        }
        if (isOnVertical(s, e->pos())) {
            setCursor(Qt::SizeHorCursor);
        } else if (isOnHorizontal(s, e->pos())) {
            setCursor(Qt::SizeVerCursor);
        //} else if (s.contains(e->pos())) {
        //    setCursor(QCursor(Qt::SizeAllCursor));
        } else {
            unsetCursor();
        }
    }
}

void LightPreviewWidget::mousePressEvent(QMouseEvent *e) {
    if (e->button() != Qt::LeftButton) return;
    QRect sRect(toScreenRect(selection));
    QPoint iClick(toPhysicalPoint(e->pos()));
    QRect image(0, 0, image.width(), image.height());
    left = right = top = bottom = move = false;
    if (isOnNw(sRect, e->pos())) {
        dragStart = iClick;
        preDragSelection = selection;
        top = left = true;
    } else if (isOnNe(sRect, e->pos())) {
        dragStart = iClick;
        preDragSelection = selection;
        top = right = true;
    } else if (isOnSw(sRect, e->pos())) {
        dragStart = iClick;
        preDragSelection = selection;
        bottom = left = true;
    } else if (isOnSe(sRect, e->pos())) {
        dragStart = iClick;
        preDragSelection = selection;
        bottom = right = true;
    } else if (isOnLeft(sRect, e->pos())) {
        dragStart = iClick;
        preDragSelection = selection;
        left = true;
    } else if (isOnRight(sRect, e->pos())) {
        dragStart = iClick;
        preDragSelection = selection;
        right = true;
    } else if (isOnTop(sRect, e->pos())) {
        dragStart = iClick;
        preDragSelection = selection;
        top = true;
    } else if (isOnBottom(sRect, e->pos())) {
        dragStart = iClick;
        preDragSelection = selection;
        bottom = true;
    /* This is for moving the rectangle. I don't need it in this class, so it's disabled.
    } else if (sRect.contains(e->pos(), true)) {
        dragStart = iClick;
        preDragSelection = selection;
        move = true;
    */
    } else if (image.contains(iClick)) {
        selection.setRect(iClick.x(), iClick.y(), 1, 1);
        dragStart = iClick;
        preDragSelection = selection;
        right = bottom = true;
        emit selectionChanged(selection);
        update();
    }
}
