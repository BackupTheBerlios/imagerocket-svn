/* PixmapView
A widget which displays images in a scrollable container at any zoom level.
Copyright (C) 2005 Wesley Crossman
Email: wesley@crossmans.net

All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, provided that the above copyright notice(s) and this permission notice appear in all copies of the Software and that both the above copyright notice(s) and this permission notice appear in supporting documentation.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Except as contained in this notice, the name of a copyright holder shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Software without prior written authorization of the copyright holder.
*/

#include "PixmapView.h"
#include "PixmapDividedZoomer.h"
#include <QtGui>
#include <iostream>
#include <assert.h>

#undef DRAWING_SPEED_TEST //broken
#define PRELOAD_MSEC 1000
#define SINGLE_STEP 25

/*!
  \class PixmapView
  \short A scrolling and zooming image view widget
  
  This widget can display large images at any zoom; optionally fit the image to the widget; and
  scroll large images with scrollbars, arrow keys, and middle-button dragging. For zooming,
  The widget uses PixmapDividedZoomer to provide pieces of the zoomed image.
  \sa PixmapDividedZoomer
 */

PixmapView::PixmapView(QWidget *parent, int pieceSize)
            : QAbstractScrollArea(parent) {
    zoom = 1.0;
    inResizeEvent = false;
    blockDrawing = false;
    brokenImage = false;
    fitToWidget = false;
    PixmapView::pieceSize = pieceSize;
    squares = NULL;
    middleButtonScrollPoint = QPoint(-1, -1);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    horizontalScrollBar()->setSingleStep(SINGLE_STEP);
    verticalScrollBar()->setSingleStep(SINGLE_STEP);
    
    //no background color on viewport
    QPalette p;
    p.setBrush(QPalette::Base, QBrush());
    viewport()->setPalette(p);
    
    pix = QPixmap();
    transparency = false;
    
    preloader = new QTimer(this);
    connect(preloader, SIGNAL(timeout()),
            this, SLOT(preloaderTimeout()));
    
    createBorders();
    
#ifdef DRAWING_SPEED_TEST
    scrollingDown = true;
    scrollingTest = new QTimer(this);
    scrollingTest->start(7000, true);
    connect(scrollingTest, SIGNAL(timeout()),
            this, SLOT(scrollingTestTimeout()));
#endif
}

PixmapView::~PixmapView() {
    delete squares;
}

void PixmapView::createBorders() {
    //Full length borders on big/zoomed images crash on Windows,so we just tile the
    //border. For the border pattern, I must also use a hack, since the pattern drawing
    //mechanism is inconsistent between OSes, at least on Qt 4.0.0. - WJC
    const int cornerCapSize = 5;
    const int borderSize = 384;
    QColor orange(255, 191, 0);
    QPen pen(Qt::darkGray), pen2(Qt::yellow);
    QPen penCorner(orange);
    QPainter p;
    
    QPixmap pix(borderSize, 1);
    p.begin(&pix);
    p.setPen(pen);
    p.drawLine(0, 0, borderSize, 0);
    p.setPen(pen2);
    for (int a=0;a<borderSize;a+=8) {
        p.drawLine(a, 0, a+3, 0);
    }
    p.end();
    horizontalBorder = pix;
    
    pix = QPixmap(1, borderSize);
    p.begin(&pix);
    p.setPen(pen);
    p.drawLine(0, 0, 0, borderSize);
    p.setPen(pen2);
    for (int a=0;a<borderSize;a+=8) {
        p.drawLine(0, a, 0, a+3);
    }
    p.end();
    verticalBorder = pix;
    
    pix = QPixmap(cornerCapSize, cornerCapSize);
    pix.fill(QColor(0, 0, 0, 0));
    p.begin(&pix);
    p.setPen(penCorner);
    p.drawRect(0, 0, cornerCapSize, cornerCapSize);
    p.end();
    nwCorner = pix;
    
    pix = QPixmap(cornerCapSize, cornerCapSize);
    pix.fill(QColor(0, 0, 0, 0));
    p.begin(&pix);
    p.setPen(penCorner);
    p.drawRect(-1, 0, cornerCapSize, cornerCapSize);
    p.end();
    neCorner = pix;
    
    pix = QPixmap(cornerCapSize, cornerCapSize);
    pix.fill(QColor(0, 0, 0, 0));
    p.begin(&pix);
    p.setPen(penCorner);
    p.drawRect(0, -1, cornerCapSize, cornerCapSize);
    p.end();
    swCorner = pix;
    
    pix = QPixmap(cornerCapSize, cornerCapSize);
    pix.fill(QColor(0, 0, 0, 0));
    p.begin(&pix);
    p.setPen(penCorner);
    p.drawRect(-1, -1, cornerCapSize, cornerCapSize);
    p.end();
    seCorner = pix;
    
    /* This code fails on Qt/Windows 4.0.1. It could replace the code above when Qt is fixed. - WJC
    QMatrix matrix;
    
    QPixmap pix(borderSize, 1);
    p.begin(&pix);
    p.setPen(pen);
    p.drawLine(0, 0, borderSize, 0);
    p.setPen(pen2);
    for (int a=0;a<borderSize;a+=8) {
        p.drawLine(a, 0, a+3, 0);
    }
    p.end();
    horizontalBorder = pix;
    matrix.rotate(90);
    verticalBorder = horizontalBorder.transformed(matrix);
    
    matrix.reset();
    QPixmap pix2(cornerCapSize, cornerCapSize);
    pix2.fill(QColor(0, 0, 0, 0));
    p.begin(&pix2);
    p.setPen(penCorner);
    p.drawRect(0, 0, cornerCapSize, cornerCapSize);
    p.end();
    nwCorner = pix2;
    matrix.rotate(90);
    neCorner = nwCorner.transformed(matrix);
    matrix.rotate(90);
    seCorner = nwCorner.transformed(matrix);
    matrix.rotate(90);
    swCorner = nwCorner.transformed(matrix);
    */
}

//! This loads the file and displays it. The widget displays error text if the load fails.
void PixmapView::load(QString fileName) {
    load(QImage(fileName));
}

//! This sets the given image to be displayed.
/*!
  The QImage is converted to a QPixmap and is displayed.
  The current zoom factor or fit-to-window is not changed.
  If the image isNull, a file-is-broken message will be displayed.
  \sa load(QPixmap, bool)
  \sa resetToBlank()
*/
void PixmapView::load(QImage newImage) {
    if (!newImage.isNull()) {
        QPixmap p(QPixmap::fromImage(newImage));
        load(p, newImage.hasAlphaChannel());
    } else {
        load(QPixmap(), false);
    }
}

//! This sets the given image to be displayed.
/*!
  The current zoom factor or fit-to-window is not changed.
  If the pixmap isNull, a file-is-broken message will be displayed.
  \param hasTransparency This cannot be determined from the pixmap
  (due to a Qt 4 bug/flaw) but can be found using QImage, if you load the file
  into it first. If in doubt, this should be True for a slightly slower but
  non-weird appearance.
  \sa load(QImage)
  \sa resetToBlank()
*/
void PixmapView::load(QPixmap newPixmap, bool hasTransparency) {
    bool changed = (newPixmap.size() != pix.size());
    transparency = hasTransparency;
    delete squares;
    squares = NULL;
    preloader->stop();
    preloadPoints.clear();
    if (!newPixmap.isNull()) {
        brokenImage = false;
        pix = newPixmap;
        squares = new PixmapDividedZoomer(&pix, transparency, pieceSize);
        if (fitToWidget) {
            updateZoomForSize();
        } else {
            resizeContents(int(pix.width() * zoom), int(pix.height() * zoom));
        }
    } else {
        brokenImage = true;
        pix = QPixmap();
        resizeContents(0, 0);
    }
    viewport()->update();
}

//! This sets the displayed image to a blank one, without error message.
void PixmapView::resetToBlank() {
    brokenImage = false;
    delete squares;
    squares = NULL;
    pix = QPixmap();
    setZoom(1.0);
}

//! This sets the parameters of the scrollbars.
void PixmapView::resizeContents(int w, int h) {
    preloadPoints.clear();
    int iw=0, ih=0;
    if (squares) {
        iw = squares->getScaledWidth();
        ih = squares->getScaledHeight();
    }
    
    int vw = viewport()->width(), vh = viewport()->height();
    QSize vSize = viewport()->size();
    QSize maxSize = maximumViewportSize();
    int rw = w - vw, rh = h - vh;
    blockDrawing = true;
    if (iw <= maxSize.width() && ih <= maxSize.height()) {
        //This is necessary due to a QAbstractArea flaw which fails
        //to hide both if they are both barely eligible to be hidden.
        horizontalScrollBar()->setRange(0, 0);
        verticalScrollBar()->setRange(0, 0);
    } else {
        QScrollBar *hs = horizontalScrollBar();
        QScrollBar *vs = verticalScrollBar();
        hs->setRange(0, rw);
        hs->setPageStep(vw);
        vs->setRange(0, rh);
        vs->setPageStep(vh);
    }
    blockDrawing = false;
}

void PixmapView::resizeEvent(QResizeEvent *e) {
    if (!inResizeEvent) {
        inResizeEvent = true;
        //qDebug("resizeEvent %d", fitToWidget);
        if (squares && fitToWidget) {
            updateZoomForSize();
        } else {
            resizeContents(int(pix.width() * zoom), int(pix.height() * zoom));
        }
        inResizeEvent = false;
    }
}

//! This sets the zoom so the image fits in the widget.
/*!
  This is the basis for the fit-to-widget feature.
\sa setFitToWidget(bool)
*/
void PixmapView::updateZoomForSize() {
    //qDebug("updateZoomForSize");
    QSize w(size()), i(pix.size()), padding(10, 10);
    w -= padding;
    QSize tmp(i);
    tmp.scale(w, Qt::KeepAspectRatio);
    double z = double(tmp.width()) / i.width();
    setZoom(z ? z : .01);
}

//! This controls whether the image is zoomed to always fit the widget.
void PixmapView::setFitToWidget(bool value) {
    fitToWidget = value;
    if (fitToWidget && !pix.isNull()) {
        updateZoomForSize();
        resizeContents(int(pix.width() * zoom), int(pix.height() * zoom));
    }
    update();
}

void PixmapView::mousePressEvent(QMouseEvent *e) {
    if (e->button() == Qt::MidButton) {
        middleButtonScrollPoint = e->pos();
        setCursor(Qt::SizeAllCursor);
    }
}

void PixmapView::mouseReleaseEvent(QMouseEvent *e) {
    setCursor(Qt::ArrowCursor);
    middleButtonScrollPoint = QPoint(-1, -1);
}

void PixmapView::mouseMoveEvent(QMouseEvent *e) {
    if (middleButtonScrollPoint.x() > -1) {
        int newX = e->x();
        int newY = e->y();
        int dx = middleButtonScrollPoint.x() - newX;
        int dy = middleButtonScrollPoint.y() - newY;
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() + dx);
        verticalScrollBar()->setValue(verticalScrollBar()->value() + dy);
        middleButtonScrollPoint = QPoint(newX, newY);
    }
}

double PixmapView::getZoom() {
    return zoom;
}

//! This centers the view on the position.
void PixmapView::center(int x, int y) {
    blockDrawing = true;
    int centerX = x - horizontalScrollBar()->pageStep()/2;
    int centerY = y - verticalScrollBar()->pageStep()/2;
    horizontalScrollBar()->setValue(centerX);
    verticalScrollBar()->setValue(centerY);
    blockDrawing = false;
}

void PixmapView::center(QPoint point) {
    center(point.x(), point.y());
}

//! This finds the visible center position in natural image coords.
/*! This position, times the current zoom, is the default zoom point for #setZoom. */
QPoint PixmapView::visibleCenter() {
    QScrollBar *scrH = horizontalScrollBar(), *scrV = verticalScrollBar();
    int visW = viewport()->width(), visH = viewport()->height();
    int pixW = squares->getScaledWidth();
    int pixH = squares->getScaledHeight();
    bool scrollingX = (visW <= pixW);
    bool scrollingY = (visH <= pixH);
    int centerX = 0, centerY = 0;
    if (scrollingX) {
        centerX = int(scrH->value() / zoom + scrH->pageStep() / zoom / 2);
    } else {
        //Non-scrolling to scrolling needs a different formula.
        centerX = pix.width() / 2;
    }
    if (scrollingY) {
        centerY = int(scrV->value() / zoom + scrV->pageStep() / zoom / 2);
    } else {
        centerY = pix.height() / 2;
    }
    return QPoint(centerX, centerY);
}

//! This sets the zoom factor and updates the widget.
/*!
  The center position can be specified, overriding the zoom into the visible center.
  Avoid unnecessary calls, since all is recalculated each time.
*/
void PixmapView::setZoom(double zoomFactor, int x, int y) {
    //qDebug("setZoom %f", zoomFactor);
    assert(zoomFactor > 0.0);
    preloadPoints.clear();
    if (!squares) {
        zoom = 1.0;
        resizeContents(0, 0);
        emit zoomChanged(zoom);
        return;
    }
    QPoint centerPoint(visibleCenter());
    squares->setZoom(zoomFactor);
    //qDebug("Total Squares: %d", squares->getPieceCount());
    resizeContents(int(pix.width() * zoomFactor), int(pix.height() * zoomFactor));
    int cx = int((x == -1) ? centerPoint.x() * zoomFactor : x * zoomFactor);
    int cy = int((y == -1) ? centerPoint.y() * zoomFactor : y * zoomFactor);
    center(cx, cy);
    viewport()->update();
    zoom = zoomFactor;
    emit zoomChanged(zoom);
}

void PixmapView::setZoom(double zoomFactor, QPoint zoomCenter) {
    setZoom(zoomFactor, zoomCenter.x(), zoomCenter.y());
}

//! This resets the zoom and position. If fit-to-widget is on, the call does nothing.
void PixmapView::resetZoomAndPosition() {
    //qDebug("resetZoomAndPosition %d", fitToWidget);
    //With fitToWidget, zoom should already be determined, and there should be no scrollbars.
    if (!fitToWidget) {
        setZoom(1.0);
        blockDrawing = true;
        horizontalScrollBar()->setValue(0);
        verticalScrollBar()->setValue(0);
        blockDrawing = false;
    }
}

void PixmapView::scrollingTestTimeout() {
    //broken
#ifdef DRAWING_SPEED_TEST
    for (int a=0;a<10;++a) {
        if ( (contentsY() >= pix.height() - visibleHeight())
             && scrollingDown) {
            scrollingDown = false;
        } else if ( (contentsY() == 0) && !scrollingDown) {
            scrollingDown = true;
        }
        scrollBy(0, scrollingDown ? 1 : -1);
    }
    scrollingTest->start(75, true);
#endif
}

//! This caches the tiles just outside the border after a few hundred msecs for speed.
void PixmapView::preloaderTimeout() {
    preloader->stop();
    if (squares && preloadSize == squares->getGridSize()) {
        qDebug("Timer Fired");
        foreach (QPoint point, preloadPoints) {
            squares->setCached(point.x(), point.y(), true);
        }
    }
    preloadPoints.clear();
}

void PixmapView::paintEvent(QPaintEvent *e) {
    if (blockDrawing) {
        return;
    }
    QPainter p(viewport());
    int clipX = e->rect().x(), clipY = e->rect().y();
    int clipW = e->rect().width(), clipH = e->rect().height();
    int visW = viewport()->width(), visH = viewport()->height();
    if (!squares) {
        p.fillRect(clipX, clipY, clipW, clipH, Qt::gray);
        if (brokenImage) {
            QFont f;
            f.setPointSize(16);
            p.setFont(f);
            p.drawText(50, 50, tr("File Broken"));
        }
        return;
    }
    int conX = horizontalScrollBar()->value(), conY = verticalScrollBar()->value();
    int scaledW = squares->getScaledWidth();
    int scaledH = squares->getScaledHeight();
    bool scrollingX = (visW <= scaledW);
    bool scrollingY = (visH <= scaledH);
    int centerX = visW / 2 - scaledW / 2;
    int centerY = visH / 2 - scaledH / 2;
    int pixX = scrollingX ? -conX : centerX;
    int pixY = scrollingY ? -conY : centerY;
    QRegion visible(0, 0, visW, visH);
    QRegion image(pixX, pixY, scaledW, scaledH);
    QRect imageWithBorderRect(pixX-1, pixY-1, scaledW+2, scaledH+2);
    QRegion imageWithBorder(imageWithBorderRect);
    
    //Borders and Corners
    if (!scrollingY) {
        p.drawTiledPixmap(pixX-1, pixY-1, scaledW+2, 1, horizontalBorder);
        p.drawTiledPixmap(pixX-1, pixY+scaledH, scaledW+2, 1, horizontalBorder);
    }
    if (!scrollingX) {
        p.drawTiledPixmap(pixX-1, pixY-1, 1, scaledH+2, verticalBorder);
        p.drawTiledPixmap(pixX+scaledW, pixY-1, 1, scaledH+2, verticalBorder);
    }
    int cornerSize = nwCorner.width();
    p.drawPixmap(pixX-1, pixY-1, nwCorner);
    p.drawPixmap(pixX+scaledW-cornerSize+1, pixY-1, neCorner);
    p.drawPixmap(pixX-1, pixY+scaledH-cornerSize+1, swCorner);
    p.drawPixmap(pixX+scaledW-cornerSize+1, pixY+scaledH-cornerSize+1, seCorner);
    
    p.setClipRegion(visible.subtract(imageWithBorder));
    p.fillRect(0, 0, visW, visH, Qt::gray);
    
    p.setClipRect(e->rect());
    bool zoomOne = (zoom > .9999 && zoom < 1.0001);
    if (zoomOne && !transparency) {
        p.drawPixmap(clipX, clipY, pix, clipX - pixX, clipY - pixY, clipW, clipH);
    } else {
        int ps = squares->getPieceSize();
        QRect contents(conX, conY, visW, visH);
        QRect preload(contents.x() - ps / 2, contents.y() - ps / 2,
                contents.width() + ps, contents.height() + ps);
        QVector < QPoint > points;
        preloadPoints.clear();
        preloadSize = squares->getGridSize();
        //create squares
        for (int x=0;x<squares->getGridWidth();++x) {
            for (int y=0;y<squares->getGridHeight();++y) {
                QSize size(squares->getSize(x, y));
                QRect sr(x * ps, y * ps, size.width(), size.height());
                if (contents.intersects(sr)) {
                    //qDebug("Loading %d,%d", x, y);
                    squares->setCached(x, y, true);
                    points.append(QPoint(x, y));
                } else if (preload.intersects(sr)) {
                    preloadPoints.append(QPoint(x, y));
                } else {
                    //qDebug("Unloading %d,%d", x, y);
                    squares->setCached(x, y, false);
                }
            }
        }
        //draw squares
        foreach (QPoint point, points) {
            QPixmap *i = squares->getPiece(point.x(), point.y());
            p.drawPixmap(pixX + point.x() * ps, pixY + point.y() * ps, *i);
        }
        preloader->start(PRELOAD_MSEC);
    }
}

/* This is junk code that I might decide on using after all. - WJC
QPixmap *PixmapView::makeTransparentTile(
       unsigned int size, int squares,
       const QColor &first, const QColor &second) {
    QPixmap *pix = new QPixmap(size*squares, size*squares);
    QPainter paint(pix);
    //paint.setPen(first);
    paint.fillRect(0, 0, size*squares, size*squares, first);
    //paint.setPen(second);
    for (int x=0;x<squares;++x) {
        for (int y=0;y<squares;++y) {
            if ( (x+y) % 2 ) {
                paint.fillRect(x*size, y*size, size, size, second);
            }
        }
    }
    return pix;
}
*/

