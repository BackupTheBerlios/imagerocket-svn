/* PixmapDividedZoomer
A class which takes a pixmap and provides pieces of the images zoomed to any size.
Copyright (C) 2005 Wesley Crossman
Email: wesley@crossmans.net

All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, provided that the above copyright notice(s) and this permission notice appear in all copies of the Software and that both the above copyright notice(s) and this permission notice appear in supporting documentation.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Except as contained in this notice, the name of a copyright holder shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Software without prior written authorization of the copyright holder.
*/

#include "PixmapDividedZoomer.h"
#include <QPainter>
#include <QSettings>
#include <QTime>
#include <limits>
#include <assert.h>

/*!
  \class PixmapDividedZoomer
  \brief A class which takes a pixmap and provides pieces of the images zoomed to any size
  
  The zoom should be set using setZoom(double). The pieces can then be created and deleted
  using the setCached(x, y, bool) function. If the image has transparency, the squares will
  be created with a transparency background that ignores the zoom factor.
  
  Positions are grid-based.
*/

int inMemory = 0;

//DEBUG class to check pixmap usage
class RocketPixmap : public QPixmap {
public:
    RocketPixmap(int w, int h) : QPixmap(w, h) {++inMemory;}
    RocketPixmap(QPixmap pix) : QPixmap(pix) {++inMemory;}
    ~RocketPixmap() {inMemory--;}
};

PixmapDividedZoomer::PixmapDividedZoomer(QPixmap *source,
            bool hasTransparency, int pieceSize) {
    assert(!inMemory); //DEBUG
    assert(source);
    PixmapDividedZoomer::source = source;
    PixmapDividedZoomer::hasTransparency = hasTransparency;
    PixmapDividedZoomer::pieceSize = pieceSize;
    scaledW = 0;
    scaledH = 0;
    if (source->hasAlphaChannel()) {
        transparent = new QPixmap(pieceSize, pieceSize);
        //TODO This should be removed and the settings should be passed with arguments,
        //so RocketView and this class can be stand-alone classes. - WJC
        QSettings settings;
        QColor first = settings.value("canvas/bgcolor1",
                                      QColor(150, 150, 150)).value<QColor>();
        QColor second = settings.value("canvas/bgcolor2",
                                      QColor(200, 200, 200)).value<QColor>();
        QPainter p(transparent);
        int squares = settings.value("canvas/squareCount", 16).toInt();
        int size = pieceSize/squares;
        p.fillRect(0, 0, pieceSize, pieceSize, first);
        for (int x=0;x<squares;++x) {
            for (int y=0;y<squares;++y) {
                if ( (x+y) % 2 ) {
                    p.fillRect(x*size, y*size, size, size, second);
                }
            }
        }
    } else {
        transparent = NULL;
    }
    setZoom(1.0);
}

PixmapDividedZoomer::~PixmapDividedZoomer() {
    if (transparent) {
        delete transparent;
    }
    for (int a=0;a<pieces.size();++a) {
        delete pieces[a];
        pieces[a] = NULL;
    }
}

//! This returns the size of the indexed piece.
QSize PixmapDividedZoomer::getSize(int x, int y) {
    int zw = int(source->width() * zoom), zh = int(source->height() * zoom);
    int zoomW = std::min(zw - x * pieceSize, pieceSize);
    int zoomH = std::min(zh - y * pieceSize, pieceSize);
    return QSize(zoomW, zoomH);
}

//! This loads or deletes a piece of the zoomed image.
/*!
  If newState matches the current state, the call is ignored.
*/
void PixmapDividedZoomer::setCached(int x, int y, bool newState) {
    //dump piece to disk: pieces[index]->save(QString("file%1.png").arg(index)), "PNG");
    assert(inMemory < 64); //DEBUG/XXX - limits size of window - remove before shipping!
    assert(x >= 0 && y >= 0);
    assert(x < getGridWidth() && y < getGridHeight());
    int index = getIndex(x, y);
    if (newState && !pieces[index]) {
        QSize scaled(getSize(x, y));
        QPixmap temp(int(scaled.width() / zoom), int(scaled.height() / zoom));
        if (hasTransparency) {
            QColor transColor(0, 0, 0, 0);
            temp.fill(transColor);
            QPainter pTemp(&temp);
            pTemp.drawPixmap(0, 0, *source,
                             int(x * pieceSize / zoom), int(y * pieceSize / zoom),
                             int(scaled.width() / zoom), int(scaled.height() / zoom));
            pieces[index] = new RocketPixmap(scaled.width(), scaled.height());
            pTemp.end();
            QPainter pPiece(pieces[index]);
            pPiece.drawPixmap(0, 0, *transparent);
            pPiece.drawPixmap(0, 0, temp.scaled(scaled.width(), scaled.height()));
        } else {
            QPainter pTemp(&temp);
            pTemp.drawPixmap(0, 0, *source,
                             int(x * pieceSize / zoom), int(y * pieceSize / zoom),
                             int(scaled.width() / zoom), int(scaled.height() / zoom));
            pieces[index] = new RocketPixmap(temp.scaled(scaled.width(), scaled.height()));
        }
        //QPainter pPiece(pieces[index]); //DEBUG draws black corner on each square
        //pPiece.drawPoint(0, 0);
        //pPiece.drawPoint(1, 0);
        //pPiece.drawPoint(0, 1);
        //qDebug("%d", index);
        //pPiece.drawText(20, 20, QString(QString("%1-")
        //        .append(QTime::currentTime().toString("h:mm:ss")))
        //        .arg(index));
    } else if (!newState && pieces[index]) {
        delete pieces[index];
        pieces[index] = NULL;
    }
}

//! This deletes all current pieces and sets the zoom.
void PixmapDividedZoomer::setZoom(double z) {
    zoom = z;
    for (int a=0;a<pieces.size();++a) {
        delete pieces[a];
        pieces[a] = NULL;
    }
    pieces.clear();
    pieceCount = (getGridWidth()-1) * getGridHeight()
            + (getGridHeight()-1) + 1;
    pieces.resize(getPieceCount());
    for (int a=0;a<pieces.size();++a) {
        pieces[a] = NULL;
    }
    scaledW = 0;
    for (int x=0;x<getGridWidth();++x) {
        scaledW += getSize(x, 0).width();
    }
    scaledH = 0;
    for (int y=0;y<getGridHeight();++y) {
        scaledH += getSize(0, y).height();
    }
    emit zoomChanged();
}
