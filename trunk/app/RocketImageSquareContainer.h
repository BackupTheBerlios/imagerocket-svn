#ifndef ROCKET_IMAGE_SQUARE_CONTAINER_H
#define ROCKET_IMAGE_SQUARE_CONTAINER_H

#include <qimage.h>
#include <qobject.h>
#include <QPixmap>
#include <vector>

class RocketImageSquareContainer : public QObject {
Q_OBJECT
protected:
    
    std::vector < QPixmap * > pieces;
    QPixmap *source;
    QPixmap *transparent;
    int pieceSize, pieceCount, scaledH, scaledW;
    bool hasTransparency;
    double zoom;
    int getIndex(int x, int y) {return (x * getGridHeight()) + y;}
    
public:
    
    RocketImageSquareContainer(QPixmap *source, bool hasTransparency, int pieceSize);
    ~RocketImageSquareContainer();
    void setCached(int x, int y, bool newState);
    int getPieceCount() {return pieceCount;}
    QSize getSize(int x, int y);
    QPixmap *getPiece(int x, int y) {
        return pieces[getIndex(x, y)];
    }
    int getGridWidth() {
        int w = int(source->width() * zoom);
        return (w / pieceSize) + ((w % pieceSize) ? 1 : 0);
    }
    int getGridHeight() {
        int h = int(source->height() * zoom);
        return (h / pieceSize) + ((h % pieceSize) ? 1 : 0);
    }
    QSize getGridSize() {
        return QSize(getGridHeight(), getGridWidth());
    }
    int getScaledWidth() {
        return scaledW;
    }
    int getScaledHeight() {
        return scaledH;
    }
    int getPieceSize() {
        return pieceSize;
    }
    double getZoom() {
        return zoom;
    }
    void setZoom(double zoom);
    
signals:
    
    void zoomChanged();

};

#endif
