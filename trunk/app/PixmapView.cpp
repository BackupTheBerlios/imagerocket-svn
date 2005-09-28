/* PixmapView
A widget which displays images in a scrollable container at any zoom level.
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

#include "PixmapView.h"
#include "PixmapDividedZoomer.h"
#include <QtGui>
#include <cassert>
#include <algorithm>

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
    middleButtonScrollPoint = QPoint(-1, -1);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    horizontalScrollBar()->setSingleStep(SINGLE_STEP);
    verticalScrollBar()->setSingleStep(SINGLE_STEP);
    
    //no background color on viewport
    QPalette palette;
    palette.setBrush(QPalette::Base, QBrush());
    viewport()->setPalette(palette);
    
    pix = QPixmap();
    transparency = false;
    
    preloader = new QTimer(this);
    connect(preloader, SIGNAL(timeout()),
            this, SLOT(preloaderTimeout()));
    
    setTransparencyPattern(MidToneChecks, 16);
    
    createBorders();
}

PixmapView::~PixmapView() {
}

void PixmapView::createBorders() {
    //Full length borders on big/zoomed images crash on Windows, so we just tile the
    //border. For the border pattern, I must also use a hack, since the pattern drawing
    //mechanism is inconsistent between OSes, at least on Qt 4.0.0. - WJC
    const int cornerCapSize = 5;
    const int borderSize = 384;
    QColor orange(255, 191, 0);
    QPen pen(Qt::darkGray), pen2(Qt::yellow);
    QPen penBorderCorner(orange);
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
    p.setPen(penBorderCorner);
    p.drawRect(0, 0, cornerCapSize, cornerCapSize);
    p.end();
    nwBorderCorner = pix;
    
    pix = QPixmap(cornerCapSize, cornerCapSize);
    pix.fill(QColor(0, 0, 0, 0));
    p.begin(&pix);
    p.setPen(penBorderCorner);
    p.drawRect(-1, 0, cornerCapSize, cornerCapSize);
    p.end();
    neBorderCorner = pix;
    
    pix = QPixmap(cornerCapSize, cornerCapSize);
    pix.fill(QColor(0, 0, 0, 0));
    p.begin(&pix);
    p.setPen(penBorderCorner);
    p.drawRect(0, -1, cornerCapSize, cornerCapSize);
    p.end();
    swBorderCorner = pix;
    
    pix = QPixmap(cornerCapSize, cornerCapSize);
    pix.fill(QColor(0, 0, 0, 0));
    p.begin(&pix);
    p.setPen(penBorderCorner);
    p.drawRect(-1, -1, cornerCapSize, cornerCapSize);
    p.end();
    seBorderCorner = pix;
    
    /* This code fails on Qt/Windows 4.0.1. It should replace the code above when Qt is fixed.
    This is issue 85128 on TrollTech's Task Tracker. - WJC
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
    p.setPen(penBorderCorner);
    p.drawRect(0, 0, cornerCapSize, cornerCapSize);
    p.end();
    nwBorderCorner = pix2;
    matrix.rotate(90);
    neBorderCorner = nwBorderCorner.transformed(matrix);
    matrix.rotate(90);
    seBorderCorner = nwBorderCorner.transformed(matrix);
    matrix.rotate(90);
    swBorderCorner = nwBorderCorner.transformed(matrix);
    */
}

//! This loads the file and displays it. The widget displays error text if the load fails.
void PixmapView::load(const QString &fileName) {
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
void PixmapView::load(const QImage &newImage) {
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
void PixmapView::load(const QPixmap &newPixmap, bool hasTransparency) {
    bool changed = (newPixmap.size() != pix.size());
    transparency = hasTransparency;
    preloader->stop();
    preloadPoints.clear();
    if (!newPixmap.isNull()) {
        brokenImage = false;
        pix = newPixmap;
        squares.setPixmap(pix, transparentTile, hasTransparency);
        if (fitToWidget) {
            updateZoomForSize();
        } else {
            resizeContents(int(pix.width() * zoom), int(pix.height() * zoom));
        }
    } else {
        brokenImage = true;
        pix = QPixmap();
        squares.reset();
        resizeContents(0, 0);
    }
    viewport()->update();
}

//!This sets the pattern that shows through images that have transparency.
/*!
  The default is MidToneChecks.\n
  patternSquareCount is -1 by default, indicating that the previous value should be used.
  \sa getPatternSquareCount()
*/
void PixmapView::setTransparencyPattern(TransparencyPattern pattern, int patternSquareCount) {
    transparencyPattern = pattern;
    if (patternSquareCount != -1) {
        this->patternSquareCount = patternSquareCount;
    }
    switch (pattern) {
    case DarkChecks:
        setCheckPattern(QColor(0, 0, 0), QColor(51, 51, 51));
        break;
    case MidToneChecks:
        setCheckPattern(QColor(102, 102, 102), QColor(153, 153, 153));
        break;
    case LightChecks:
        setCheckPattern(QColor(204, 204, 204), QColor(255, 255, 255));
        break;
    case Black:
        transparentTile = QPixmap(pieceSize, pieceSize);
        transparentTile.fill(Qt::black);
        break;
    case Gray:
        transparentTile = QPixmap(pieceSize, pieceSize);
        transparentTile.fill(Qt::gray);
        break;
    case White:
        transparentTile = QPixmap(pieceSize, pieceSize);
        transparentTile.fill(Qt::white);
        break;
    default:
        assert(0);
        break;
    }
    if (!pix.isNull()) {
        //XXX check that this doesn't change position or cause any other side-effects - WJC
        load(pix, transparency);
    }
}

//! This is a helper function for #setTransparencyPattern.
void PixmapView::setCheckPattern(QColor one, QColor two) {
    transparentTile = QPixmap(pieceSize, pieceSize);
    QPainter paint(&transparentTile);
    int size = pieceSize/patternSquareCount;
    paint.fillRect(0, 0, pieceSize, pieceSize, one);
    for (int x=0;x<patternSquareCount;++x) {
        for (int y=0;y<patternSquareCount;++y) {
            if ( (x+y) % 2 ) {
                paint.fillRect(x*size, y*size, size, size, two);
            }
        }
    }
}

//! This sets the displayed image to a blank one, without error message.
void PixmapView::resetToBlank() {
    brokenImage = false;
    pix = QPixmap();
    squares.reset();
    setZoom(1.0);
}

//! This sets the parameters of the scrollbars.
void PixmapView::resizeContents(int w, int h) {
    preloadPoints.clear();
    int iw=0, ih=0;
    if (!pix.isNull()) {
        iw = squares.getScaledWidth();
        ih = squares.getScaledHeight();
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

/*!
  This resizes the underlying image if fitToWidget is true.
  \sa setFitToWidget(bool)
*/
void PixmapView::resizeEvent(QResizeEvent *e) {
    if (!inResizeEvent) {
        inResizeEvent = true;
        if (!pix.isNull() && fitToWidget) {
            updateZoomForSize();
        } else {
            //updates scrollbars
            resizeContents(int(pix.width() * zoom), int(pix.height() * zoom));
        }
        inResizeEvent = false;
    }
}

//! This sets the zoom so the image fits in the widget.
/*!
  This is the working function for the fit-to-widget feature.
\sa setFitToWidget(bool)
*/
void PixmapView::updateZoomForSize() {
    QSize w(size()), i(pix.size()), padding(10, 10);
    w -= padding;
    QSize tmp(i);
    tmp.scale(w, Qt::KeepAspectRatio);
    double z = double(tmp.width()) / i.width();
    setZoom(z ? z : .01);
}

//! This controls whether the image is zoomed to always fit the widget.
/*!
  \sa updateZoomForSize()
*/
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

//! This centers the view on the position, using viewport coords.
void PixmapView::center(int x, int y) {
    blockDrawing = true;
    int centerX = x - horizontalScrollBar()->pageStep()/2;
    int centerY = y - verticalScrollBar()->pageStep()/2;
    horizontalScrollBar()->setValue(centerX);
    verticalScrollBar()->setValue(centerY);
    blockDrawing = false;
}

//! This finds the visible center position in natural image coords.
/*! This position, times the current zoom, is the default zoom point for #setZoom. */
QPoint PixmapView::visibleCenter() const {
    QScrollBar *scrH = horizontalScrollBar(), *scrV = verticalScrollBar();
    int visW = viewport()->width(), visH = viewport()->height();
    int pixW = squares.getScaledWidth();
    int pixH = squares.getScaledHeight();
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
    if (pix.isNull()) {
        zoom = 1.0;
        resizeContents(0, 0);
    } else {
        QPoint centerPoint(visibleCenter());
        squares.setZoom(zoomFactor);
        //qDebug("Total Squares: %d", squares.getPieceCount());
        resizeContents(int(pix.width() * zoomFactor), int(pix.height() * zoomFactor));
        int cx = int((x == -1) ? centerPoint.x() * zoomFactor : x * zoomFactor);
        int cy = int((y == -1) ? centerPoint.y() * zoomFactor : y * zoomFactor);
        center(cx, cy);
        viewport()->update();
        zoom = zoomFactor;
    }
    emit zoomChanged(zoom);
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

/*!
  This caches the tiles just outside the border after a few hundred msecs of inactivity for speed.
*/
void PixmapView::preloaderTimeout() {
    preloader->stop();
    if (!pix.isNull() && preloadSize == squares.getGridSize()) {
        qDebug("Timer Fired");
        foreach (QPoint point, preloadPoints) {
            squares.setCached(point, true);
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
    if (pix.isNull()) {
        p.fillRect(clipX, clipY, clipW, clipH, palette().mid());
        if (brokenImage) {
            QFont f;
            f.setPointSize(16);
            p.setFont(f);
            p.drawText(50, 50, tr("File Broken"));
        }
        return;
    }
    int conX = horizontalScrollBar()->value(), conY = verticalScrollBar()->value();
    int scaledW = squares.getScaledWidth();
    int scaledH = squares.getScaledHeight();
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
    
    //Borders and BorderCorners
    if (!scrollingY) {
        p.drawTiledPixmap(pixX-1, pixY-1, scaledW+2, 1, horizontalBorder);
        p.drawTiledPixmap(pixX-1, pixY+scaledH, scaledW+2, 1, horizontalBorder);
    }
    if (!scrollingX) {
        p.drawTiledPixmap(pixX-1, pixY-1, 1, scaledH+2, verticalBorder);
        p.drawTiledPixmap(pixX+scaledW, pixY-1, 1, scaledH+2, verticalBorder);
    }
    int cornerSize = nwBorderCorner.width();
    p.drawPixmap(pixX-1, pixY-1, nwBorderCorner);
    p.drawPixmap(pixX+scaledW-cornerSize+1, pixY-1, neBorderCorner);
    p.drawPixmap(pixX-1, pixY+scaledH-cornerSize+1, swBorderCorner);
    p.drawPixmap(pixX+scaledW-cornerSize+1, pixY+scaledH-cornerSize+1, seBorderCorner);
    
    p.setClipRegion(visible.subtract(imageWithBorder));
    p.fillRect(0, 0, visW, visH, palette().mid());
    
    p.setClipRect(e->rect());
    bool zoomOne = (zoom > .9999 && zoom < 1.0001);
    if (zoomOne && !transparency) {
        p.drawPixmap(clipX, clipY, pix, clipX - pixX, clipY - pixY, clipW, clipH);
    } else {
        QSize tile = squares.getMaximumPieceSize();
        QRect contents(conX, conY, visW, visH);
        QRect preload(contents.x() - tile.width() / 2, contents.y() - tile.height() / 2,
                contents.width() + tile.width(), contents.height() + tile.height());
        QVector < QPoint > points;
        preloadPoints.clear();
        preloadSize = squares.getGridSize();
        //create squares
        for (int x=0;x<squares.getGridWidth();++x) {
            for (int y=0;y<squares.getGridHeight();++y) {
                QSize size(squares.getPieceSize(x, y));
                QRect sr(x * tile.width(), y * tile.height(), size.width(), size.height());
                if (contents.intersects(sr)) {
                    //qDebug("Loading %d,%d", x, y);
                    squares.setCached(x, y, true);
                    points.append(QPoint(x, y));
                } else if (preload.intersects(sr)) {
                    preloadPoints.append(QPoint(x, y));
                } else {
                    //qDebug("Unloading %d,%d", x, y);
                    squares.setCached(x, y, false);
                }
            }
        }
        //draw squares
        foreach (QPoint point, points) {
            const QPixmap *i = squares.getPiece(point.x(), point.y());
            p.drawPixmap(pixX + point.x() * tile.width(), pixY + point.y() * tile.height(), *i);
        }
        preloader->start(PRELOAD_MSEC);
    }
}
