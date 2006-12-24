/* PixmapDividedZoomer
A class which takes a pixmap and provides pieces of the images zoomed to any size
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

#ifndef PIXMAP_DIVIDED_ZOOMER_H
#define PIXMAP_DIVIDED_ZOOMER_H

#include <QObject>
#include <QVector>
#include <QPixmap>
#include <QImage>
#include <cassert>

class PixmapDividedZoomer : public QObject {
Q_OBJECT
protected:
    
    QVector < QPixmap * > pieces;
    QPixmap source, transparent;
    int pieceCount, scaledH, scaledW;
    bool hasTransparency;
    double zoom;
    int getIndex(int x, int y) const {return (x * getGridHeight()) + y;}
    void resetArray();
    
public:
    
    PixmapDividedZoomer();
    ~PixmapDividedZoomer();
    
    void setPixmap(const QPixmap &source, const QPixmap &transparent, bool hasTransparency);
    bool isNull() const {return source.isNull();}
    
    void setZoom(double zoom);
    double getZoom() const {
        assert(!isNull());
        return zoom;
    }
    
    void setCached(int x, int y, bool newState);
    //! Convenience function for setCached(int, int, bool)
    void setCached(QPoint p, bool newState) {setCached(p.x(), p.y(), newState);}
    
    void reset();
    void freePieces();
    
    int getPieceCount() const {return pieceCount;}
    
    QSize getPieceSize(int x, int y) const;
    //! Convenience function for getPieceSize(int, int)
    QSize getPieceSize(QPoint p) const {return getPieceSize(p.x(), p.y());}
    QSize getMaximumPieceSize() const {
        assert(!isNull());
        return transparent.size();
    }
    
    const QPixmap *getPiece(int x, int y) const {
        assert(!isNull());
        return pieces[getIndex(x, y)];
    }
    //! Convenience function for getPiece(int, int)
    const QPixmap *getPiece(QPoint p) const {return getPiece(p.x(), p.y());}
    
    int getGridWidth() const {
        assert(!isNull());
        int w = int(source.width() * zoom);
        int pieceW = transparent.width();
        return (w / pieceW) + ((w % pieceW) ? 1 : 0);
    }
    int getGridHeight() const {
        assert(!isNull());
        int h = int(source.height() * zoom);
        int pieceH = transparent.height();
        return (h / pieceH) + ((h % pieceH) ? 1 : 0);
    }
    QSize getGridSize() const {
        assert(!isNull());
        return QSize(getGridHeight(), getGridWidth());
    }
    
    int getScaledWidth() const {
        assert(!isNull());
        return scaledW;
    }
    int getScaledHeight() const {
        assert(!isNull());
        return scaledH;
    }
    
    QSize getScaledSize() const {
        return QSize(getScaledWidth(), getScaledHeight());
    }
    
signals:
    
    void zoomChanged();

};

#endif
