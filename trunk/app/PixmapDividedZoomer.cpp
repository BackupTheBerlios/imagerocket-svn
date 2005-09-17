/* PixmapDividedZoomer
A class which takes a pixmap and provides pieces of the images zoomed to any size.
Copyright (C) 2005 Wesley Crossman
Email: wesley@crossmans.net

Note that this class may not be used on programs not under the GPL. Email me if you
wish to discuss the use of this class in closed-source programs.

You can redistribute and/or modify this software under the terms of the GNU
General Public License as published by the Free Software Foundation;
either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this
program; if not, write to the Free Software Foundation, Inc., 59 Temple Place,
Suite 330, Boston, MA 02111-1307 USA */

#include "PixmapDividedZoomer.h"
#include <QPainter>
#include <QSettings>
#include <QTime>
#include <algorithm>

/*!
  \class PixmapDividedZoomer
  \brief A class which takes a pixmap and provides pieces of the images zoomed to any size
  
  Set the source pixmap and transparent tile with #setPixmap. The zoom should be set using
  #setZoom. The pieces can then be created and deleted using the #setCached function. If
  the image has transparency, the squares will be created with a transparency background
  that ignores the zoom factor.
  
  Positions are grid-based.
*/

int inMemory = 0;

//DEBUG class to check pixmap usage
class CountingPixmap : public QPixmap {
public:
    CountingPixmap(int w, int h) : QPixmap(w, h) {++inMemory;}
    CountingPixmap(QPixmap pix) : QPixmap(pix) {++inMemory;}
    ~CountingPixmap() {inMemory--;}
};

PixmapDividedZoomer::PixmapDividedZoomer() {
    zoom = 1.0;
}

//! This sets the image, transparent tile, and whether the image has transparency.
/*!
  The transparent tile, aside from its main role, also supplies the maximum size of the pieces.\n
  If you're unsure about the proper value, hasTransparency can always be set to true, but that
  will slow the generation of pieces slightly, since the transparent tile will be drawn onto pieces
  before the image itself.
*/
void PixmapDividedZoomer::setPixmap(QPixmap &source, QPixmap &transparent, bool hasTransparency) {
    PixmapDividedZoomer::source = source;
    PixmapDividedZoomer::transparent = transparent;
    PixmapDividedZoomer::hasTransparency = hasTransparency;
    scaledW = 0;
    scaledH = 0;
    resetArray();
}

PixmapDividedZoomer::~PixmapDividedZoomer() {
    freePieces();
}

//! This returns the size of the indexed piece.
/*!
    This will be no larger the result of getMaximumPieceSize.
*/
QSize PixmapDividedZoomer::getPieceSize(int x, int y) const {
    assert(!isNull());
    int zw = int(source.width() * zoom), zh = int(source.height() * zoom);
    int zoomW = std::min(zw - x * transparent.width(), transparent.width());
    int zoomH = std::min(zh - y * transparent.height(), transparent.height());
    return QSize(zoomW, zoomH);
}

//! This loads or deletes a piece of the zoomed image.
/*!
  If newState matches the current state, the call is ignored.
*/
void PixmapDividedZoomer::setCached(int x, int y, bool newState) {
    assert(!isNull());
    assert(inMemory < 64); //DEBUG/XXX - limits size of window - remove before shipping!
    assert(x >= 0 && y >= 0);
    assert(x < getGridWidth() && y < getGridHeight());
    int index = getIndex(x, y);
    if (newState && !pieces[index]) {
        QSize scaled(getPieceSize(x, y));
        QPixmap temp(int(scaled.width() / zoom), int(scaled.height() / zoom));
        if (hasTransparency) {
            QColor transColor(0, 0, 0, 0);
            temp.fill(transColor);
            QPainter pTemp(&temp);
            pTemp.drawPixmap(0, 0, source,
                    int(x * transparent.width() / zoom), int(y * transparent.height() / zoom),
                    int(scaled.width() / zoom), int(scaled.height() / zoom));
            pieces[index] = new CountingPixmap(scaled.width(), scaled.height());
            pTemp.end();
            QPainter pPiece(pieces[index]);
            pPiece.drawPixmap(0, 0, transparent);
            pPiece.drawPixmap(0, 0, temp.scaled(scaled.width(), scaled.height()));
        } else {
            QPainter pTemp(&temp);
            pTemp.drawPixmap(0, 0, source,
                    int(x * transparent.width() / zoom), int(y * transparent.height() / zoom),
                    int(scaled.width() / zoom), int(scaled.height() / zoom));
            pieces[index] = new CountingPixmap(temp.scaled(scaled.width(), scaled.height()));
        }
        //dump piece to disk: pieces[index]->save(QString("file%1.png").arg(index)), "PNG");  //DEBUG
        
        //QPainter pPiece(pieces[index]); //DEBUG draws black corner on each square
        //pPiece.drawPoint(0, 0); pPiece.drawPoint(1, 0); pPiece.drawPoint(0, 1);
        //qDebug("%d", index);
        //pPiece.drawText(20, 20, QString(QString("%1-")
        //        .append(QTime::currentTime().toString("h:mm:ss")))
        //        .arg(index));
    } else if (!newState && pieces[index]) {
        delete pieces[index];
        pieces[index] = NULL;
    }
}

//This makes the instance isNull and frees the pieces but does not change the zoom.
void PixmapDividedZoomer::reset() {
    source = QPixmap();
    freePieces();
}

//! This deletes all current pieces and sets the zoom.
void PixmapDividedZoomer::setZoom(double z) {
    assert(!isNull());
    zoom = z;
    resetArray();
    emit zoomChanged();
}

//! This frees pieces with #freePieces and recreates the array based on the current settings. #setZoom calls this.
void PixmapDividedZoomer::resetArray() {
    freePieces();
    pieces.clear();
    pieceCount = (getGridWidth()-1) * getGridHeight()
            + (getGridHeight()-1) + 1;
    pieces.resize(getPieceCount());
    for (int a=0;a<pieces.size();++a) {
        pieces[a] = NULL;
    }
    scaledW = 0;
    for (int x=0;x<getGridWidth();++x) {
        scaledW += getPieceSize(x, 0).width();
    }
    scaledH = 0;
    for (int y=0;y<getGridHeight();++y) {
        scaledH += getPieceSize(0, y).height();
    }
}

//! This frees all pieces allocated by #setCached.
void PixmapDividedZoomer::freePieces() {
    for (int a=0;a<pieces.size();++a) {
        delete pieces[a];
        pieces[a] = NULL;
    }
}
