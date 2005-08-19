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

#ifndef ROCKET_IMAGE_SQUARE_CONTAINER_H
#define ROCKET_IMAGE_SQUARE_CONTAINER_H

#include <QObject>
#include <QVector>
#include <QPixmap>
#include <QImage>

class RocketImageSquareContainer : public QObject {
Q_OBJECT
protected:
    
    QVector < QPixmap * > pieces;
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
