/* PixmapView
A widget which displays images in a scrollable container at any zoom level.
Copyright (C) 2005 Wesley Crossman
Email: wesley@crossmans.net

All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, provided that the above copyright notice(s) and this permission notice appear in all copies of the Software and that both the above copyright notice(s) and this permission notice appear in supporting documentation.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Except as contained in this notice, the name of a copyright holder shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Software without prior written authorization of the copyright holder.
*/

#ifndef PIXMAP_VIEW_H
#define PIXMAP_VIEW_H

#include "PixmapDividedZoomer.h"
#include <QtGui>

class PixmapView : public QAbstractScrollArea {
Q_OBJECT
public:
    
    enum TransparencyPattern {DarkChecks, MidToneChecks, LightChecks, Black, Gray, White};
    
protected:
    
    QPixmap pix, transparentTile;
    QPixmap horizontalBorder, verticalBorder, nwCorner, neCorner, swCorner, seCorner;
    bool transparency, fitToWidget, blockDrawing, brokenImage;
    PixmapDividedZoomer squares;
    double zoom;
    int pieceSize;
    bool inResizeEvent;
    QPoint middleButtonScrollPoint;
    TransparencyPattern transparencyPattern;
    
    QTimer *preloader;
    QVector < QPoint > preloadPoints;
    QSize preloadSize;
    
    void resizeContents(int w, int h);
    void updateZoomForSize();
    
    void resizeEvent(QResizeEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    
    void paintEvent(QPaintEvent *e);
    
    void createBorders();
    
    void setCheckPattern(QColor one, QColor two);
    
    //Automated scrolling stuff
    bool scrollingDown;
    QTimer *scrollingTest;
    
protected slots:
    
    void scrollingTestTimeout();
    void preloaderTimeout();
    
public:
    
    PixmapView(QWidget *parent, int pieceSize);
    virtual ~PixmapView();
    
    void load(QString fileName);
    void load(QImage newImage);
    void load(QPixmap newPixmap, bool hasTransparency);
    void resetToBlank();
    QPixmap &getPixmap() {
        return pix;
    }
    
    QSize imageSize() {return pix.size();}
    
    double getZoom();
    void setZoom(double zoomFactor, int x=-1, int y=-1);
    void setZoom(double zoomFactor, QPoint zoomCenter);
    void resetZoomAndPosition();
    
    void setTransparencyPattern(TransparencyPattern pattern);
    TransparencyPattern getTransparencyPattern();
    
    void setFitToWidget(bool);
    bool willFitToWidget() {
        return fitToWidget;
    }
    
    void center(int x, int y);
    void center(QPoint point);
    QPoint visibleCenter();
    
    bool isNullImage() {
        return pix.isNull();
    }

signals:
    
    void zoomChanged(double zoom);
    
};

#endif

