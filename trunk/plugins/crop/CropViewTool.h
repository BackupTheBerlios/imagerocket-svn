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

#ifndef CROP_VIEW_TOOL
#define CROP_VIEW_TOOL

#include "interfaces.h"
#include "PixmapView.h"

class CropViewTool : public QObject, public PixmapViewTool {
Q_OBJECT
public:
    CropViewTool();
public slots:
    void setSelection(ImageRect ir);
protected:
    QPoint dragStart;
    ImageRect selection, preDragSelection;
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
    virtual void paintEvent(QPainter &p, QPaintEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void setParent(PixmapView *parent);
signals:
    void selectionChanged(ImageRect);
    void selected();
};

#endif
