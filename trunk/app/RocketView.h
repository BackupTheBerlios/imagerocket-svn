//#include "RocketImage.h"
#include "RocketImageSquareContainer.h"
#include <QtGui>

#ifndef ROCKET_VIEW_H
#define ROCKET_VIEW_H

class RocketView : public QAbstractScrollArea {
Q_OBJECT
protected:
    
    QPixmap *pix;
    QPixmap horizontalBorder, verticalBorder, nwCorner, neCorner, swCorner, seCorner;
    bool transparency, fitToWidget, blockDrawing, brokenImage;
    RocketImageSquareContainer *squares;
    double zoom;
    int pieceSize;
    bool inResizeEvent;
    int middleButtonScrollX, middleButtonScrollY;
    
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
    
    //Automated scrolling stuff
    bool scrollingDown;
    QTimer *scrollingTest;
    
protected slots:
    
    void scrollingTestTimeout();
    void preloaderTimeout();
    
public:
    
    RocketView(QWidget *parent, int pieceSize);
    virtual ~RocketView();
    
    void load(QString fileName);
    void load(QImage newImage);
    void load(QPixmap newPixmap, bool hasTransparency);
    void resetToBlank();
    QPixmap *getPixmap() {
        return pix;
    }
    
    QSize imageSize() {return pix->size();}
    
    double getZoom();
    void setZoom(double zoomFactor, int x=-1, int y=-1);
    void setZoom(double zoomFactor, QPoint zoomCenter);
    void resetZoomAndPosition();
    
    void setFitToWidget(bool);
    bool willFitToWidget() {
        return fitToWidget;
    }
    
    void center(int x, int y);
    void center(QPoint point);
    QPoint visibleCenter();
    
    bool isNullImage() {
        return pix->isNull();
    }

signals:
    
    void zoomChanged(double zoom);
    
};

#endif

