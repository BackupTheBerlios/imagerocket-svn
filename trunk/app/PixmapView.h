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

#ifndef PIXMAP_VIEW_H
#define PIXMAP_VIEW_H

#include "PixmapDividedZoomer.h"
#include <QtGui>

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
    
protected slots:
    
    void preloaderTimeout();
    
public:
    
    PixmapView(QWidget *parent, int pieceSize);
    virtual ~PixmapView();
    
    void load(const QString &fileName);
    void load(const QImage &newImage);
    void load(const QPixmap &newPixmap, bool hasTransparency);
    void resetToBlank();
    const QPixmap &getPixmap() const {return pix;}
    
    QSize imageSize() const {return pix.size();}
    
    double getZoom() const {return zoom;}
    void setZoom(double zoomFactor, int x=-1, int y=-1);
    //! Convenience function for setZoom(double, int, int)
    void setZoom(double zoomFactor, QPoint zoomCenter) {
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
    void center(QPoint point) {
        center(point.x(), point.y());
    }
    QPoint visibleCenter() const;
    
    bool isNullImage() const {return pix.isNull();}

signals:
    
    void zoomChanged(double zoom);
    
};

#endif

