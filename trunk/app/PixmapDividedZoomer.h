/* PixmapDividedZoomer
A class which takes a pixmap and provides pieces of the images zoomed to any size.
Copyright (C) 2005 Wesley Crossman
Email: wesley@crossmans.net

All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, provided that the above copyright notice(s) and this permission notice appear in all copies of the Software and that both the above copyright notice(s) and this permission notice appear in supporting documentation.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Except as contained in this notice, the name of a copyright holder shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Software without prior written authorization of the copyright holder.
*/

#ifndef PIXMAP_DIVIDED_ZOOMER_H
#define PIXMAP_DIVIDED_ZOOMER_H

#include <QObject>
#include <QVector>
#include <QPixmap>
#include <QImage>

class PixmapDividedZoomer : public QObject {
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
    
    PixmapDividedZoomer(QPixmap *source, bool hasTransparency, int pieceSize);
    ~PixmapDividedZoomer();
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
