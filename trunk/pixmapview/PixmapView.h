/* PixmapView
A widget which displays images in a scrollable container at any zoom level
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

#ifndef PIXMAP_VIEW_H
#define PIXMAP_VIEW_H

#include "PixmapViewTool.h"
#include "PixmapDividedZoomer.h"
#include <QAbstractScrollArea>
#include <QScrollBar>
#include <QPair>

class ImagePoint : public QPoint {
public:
    ImagePoint() : QPoint() {}
    ImagePoint(int x, int y) : QPoint(x, y) {}
    static ImagePoint fromPoint(const QPoint &r) {
        return ImagePoint(r.x(), r.y());
    }
};

class PhysicalPoint : public QPoint {
public:
    PhysicalPoint() : QPoint() {}
    PhysicalPoint(int x, int y) : QPoint(x, y) {}
    static PhysicalPoint fromPoint(const QPoint &r) {
        return PhysicalPoint(r.x(), r.y());
    }
};

class ScreenPoint : public QPoint {
public:
    ScreenPoint() : QPoint() {}
    ScreenPoint(int x, int y) : QPoint(x, y) {}
    static ScreenPoint fromPoint(const QPoint &r) {
        return ScreenPoint(r.x(), r.y());
    }
};

class ImageRect : public QRect {
public:
    ImageRect() : QRect() {}
    ImageRect(int x, int y, int w, int h) : QRect(x, y, w, h) {}
    ImageRect(QPoint p, QSize s) : QRect(p, s) {}
    static ImageRect fromRect(const QRect &r) {
        ImageRect ir; ir.setTopLeft(r.topLeft()); ir.setSize(r.size()); return ir;
    }
};

class PhysicalRect : public QRect {
public:
    PhysicalRect() : QRect() {}
    PhysicalRect(int x, int y, int w, int h) : QRect(x, y, w, h) {}
    PhysicalRect(QPoint p, QSize s) : QRect(p, s) {}
    static PhysicalRect fromRect(const QRect &r) {
        PhysicalRect pr; pr.setTopLeft(r.topLeft()); pr.setSize(r.size()); return pr;
    }
};

class ScreenRect : public QRect {
public:
    ScreenRect() : QRect() {}
    ScreenRect(int x, int y, int w, int h) : QRect(x, y, w, h) {}
    ScreenRect(QPoint p, QSize s) : QRect(p, s) {}
    static ScreenRect fromRect(const QRect &r) {
        ScreenRect sr; sr.setTopLeft(r.topLeft()); sr.setSize(r.size()); return sr;
    }
};

class PixmapView : public QAbstractScrollArea {
Q_OBJECT
public:
    
    //! These are the choices for the pattern which shows behind transparent images.
    /*!
        These patterns are identical to those provided by Gimp, so I assume they've been
        thought out. - WJC
        - *Checks - checkerboard patterns with color schemes based on their prefix
        - Black, Gray, White: solid colors
    */
    enum TransparencyPattern {DarkChecks, MidToneChecks, LightChecks, Black, Gray, White};
    
protected:
    
    QPixmap pix, transparentTile;
    QPixmap horizontalBorder, verticalBorder;
    QPixmap nwBorderCorner, neBorderCorner, swBorderCorner, seBorderCorner;
    bool transparency, fitToWidget, blockDrawing, brokenImage;
    PixmapDividedZoomer squares;
    double zoom;
    int pieceSize;
    bool inResizeEvent;
    QPoint middleButtonScrollPoint;
    TransparencyPattern transparencyPattern;
    int patternSquareCount;
    PixmapViewTool *tool;
    
    QTimer *preloader;
    QVector < QPoint > preloadPoints;
    QSize preloadSize;
    
    void resizeContents(QSize size);
    void resizeContents(int w, int h) {resizeContents(QSize(w, h));}
    void updateZoomForSize();
    
    void resizeEvent(QResizeEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    
    void paintEvent(QPaintEvent *e);
    
    void createBorders();
    
    void setCheckPattern(QColor one, QColor two);
    
protected slots:
    
    void preloaderTimeout();
    
public:
    
    PixmapView(QWidget *parent, int pieceSize);
    virtual ~PixmapView();
    
    // <--
    ImagePoint toImagePoint(PhysicalPoint p) {
        return ImagePoint(int(p.x()/zoom), int(p.y()/zoom));
    }
    ImagePoint toImagePoint(ScreenPoint p) {
        return toImagePoint(toPhysicalPoint(p));
    }
    
    PhysicalPoint toPhysicalPoint(ImagePoint p) {
        return PhysicalPoint(int(p.x()*zoom), int(p.y()*zoom));
    }
    // <--
    PhysicalPoint toPhysicalPoint(ScreenPoint p) {
        int conX = horizontalScrollBar()->value(), conY = verticalScrollBar()->value();
        QSize margin(getMargin());
        return PhysicalPoint(p.x()+conX-margin.width(), p.y()+conY-margin.height());
    }
    
    ScreenPoint toScreenPoint(ImagePoint p) {
        return toScreenPoint(toPhysicalPoint(p));
    }
    ScreenPoint toScreenPoint(PhysicalPoint p) {
        int conX = horizontalScrollBar()->value(), conY = verticalScrollBar()->value();
        QSize margin(getMargin());
        return ScreenPoint(p.x()-conX+margin.width(), p.y()-conY+margin.height());
    }
    // <--
    
    // <--
    ImageRect toImageRect(PhysicalRect p) {
        return ImageRect(int(p.x()/zoom), int(p.y()/zoom), int(p.width()/zoom), int(p.height()/zoom));
    }
    ImageRect toImageRect(ScreenRect p) {
        return toImageRect(toPhysicalRect(p));
    }
    
    PhysicalRect toPhysicalRect(ImageRect p) {
        return PhysicalRect(int(p.x()*zoom), int(p.y()*zoom), int(p.width()*zoom), int(p.height()*zoom));
    }
    // <--
    PhysicalRect toPhysicalRect(ScreenRect p) {
        QPoint t(p.topLeft());
        return PhysicalRect(toPhysicalPoint(ScreenPoint(t.x(), t.y())), p.size());
    }
    
    ScreenRect toScreenRect(ImageRect p) {
        return toScreenRect(toPhysicalRect(p));
    }
    ScreenRect toScreenRect(PhysicalRect p) {
        QPoint t(p.topLeft());
        return ScreenRect(toScreenPoint(PhysicalPoint(t.x(), t.y())), p.size());
    }
    // <--
    
    void load(const QString &fileName);
    void load(const QImage &newImage);
    void load(const QPixmap &newPixmap, bool hasTransparency);
    void resetToBlank();
    const QPixmap &getPixmap() const {return pix;}
    
    QSize getImageSize() const {return pix.size();}
    
    double getZoom() const {return zoom;}
    void setZoom(double zoomFactor, int x=-1, int y=-1);
    //! Convenience function for setZoom(double, int, int)
    void setZoom(double zoomFactor, ImagePoint zoomCenter) {
        setZoom(zoomFactor, zoomCenter.x(), zoomCenter.y());
    }
    void resetZoomAndPosition();
    
    void setTransparencyPattern(TransparencyPattern pattern, int patternSquareCount = -1);
    TransparencyPattern getTransparencyPattern() const {return transparencyPattern;}
    int getPatternSquareCount() const {return patternSquareCount;}
    
    void setFitToWidget(bool);
    bool isFitToWidget() const {return fitToWidget;}
    
    void center(int x, int y);
    //! Convenience function for center(int, int)
    void center(PhysicalPoint point) {
        center(point.x(), point.y());
    }
    PhysicalPoint getVisibleCenter() const;
    QPair < bool, bool > isScrolling() const;
    QSize getMargin() const;
    
    PixmapViewTool *getTool() {
        return tool;
    }
    void setTool(PixmapViewTool *tool);
    
    bool isNullImage() const {return pix.isNull();}

signals:
    
    void zoomChanged(double zoom);
    
};

#endif

