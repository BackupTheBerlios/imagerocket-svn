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

#ifndef LIGHT_PREVIEW_WIDGET
#define LIGHT_PREVIEW_WIDGET

#include <QtGui>

class LightPreviewWidget : public QWidget {
Q_OBJECT
protected:
    
    QPoint dragStart;
    QRect selection, preDragSelection;
    QPixmap pixmap;
    
    bool left, right, top, bottom, move;
    bool isOnLeft(const QRect &r, const QPoint p);
    bool isOnRight(const QRect &r, const QPoint p);
    bool isOnTop(const QRect &r, const QPoint p);
    bool isOnBottom(const QRect &r, const QPoint p);
    bool isOnVertical(const QRect &r, const QPoint p);
    bool isOnHorizontal(const QRect &r, const QPoint p);
    bool isOnNw(const QRect &r, const QPoint p);
    bool isOnNe(const QRect &r, const QPoint p);
    bool isOnSw(const QRect &r, const QPoint p);
    bool isOnSe(const QRect &r, const QPoint p);
    bool isOnNeSw(const QRect &r, const QPoint p);
    bool isOnNwSe(const QRect &r, const QPoint p);
    
    QSize sizeHint();
    void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    
    QPoint toScreenPoint(QPoint point) {
        QSize s = size()/2 - pixmap.size()/2;
        return QPoint(s.width() + point.x(), s.height() + point.y());
    }
    QPoint toPhysicalPoint(QPoint point) {
        QSize s = size()/2 - pixmap.size()/2;
        return QPoint(point.x() - s.width(), point.y() - s.height());
    }
    QRect toScreenRect(QRect rect) {
        return QRect(toScreenPoint(rect.topLeft()), rect.size());
    }
    QRect toPhysicalRect(QRect rect) {
        return QRect(toPhysicalPoint(rect.topLeft()), rect.size());
    }
    
public:
    
    LightPreviewWidget(QWidget *parent);
    
    QPixmap getPixmap();
    
    QRect getSelection();
    void setSelection(QRect rect);
    
public slots:
    
    void setPixmap(const QPixmap &pix);
    void selectAll();
    
signals:

    void selectionChanged(QRect selection);

};

#endif
