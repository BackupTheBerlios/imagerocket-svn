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

#ifndef IMAGE_ROTATE_H
#define IMAGE_ROTATE_H

#include <cassert>

/*!
  \class ImageRotate
  \short This class substitutes for the broken rotation in Qt.
  
  It allows rotation in 90 degree increments.
*/

class ImageRotate {
public:
    static QImage rotate(int degrees, QImage src);
    static QImage rotate90(QImage src);
    static QImage rotate180(QImage src);
    static QImage rotate270(QImage src);
private:
    ImageRotate() {}
};

QImage ImageRotate::rotate(int degrees, QImage src) {
    if (degrees == 90) {
        return rotate90(src);
    } else if (degrees == 180) {
        return rotate180(src);
    } else if (degrees == 270 || degrees == -90) {
        return rotate270(src);
    } else if (degrees == 0 || degrees == 360) {
        return src;
    } else {
        assert(0); return QImage();
    }
}

QImage ImageRotate::rotate90(QImage src) {
    QImage dst(src.height(), src.width(), src.format());
    for (int x=0;x<src.width();++x) {
        for (int y=0;y<src.height();++y) {
            dst.setPixel(src.height()-y-1, x, src.pixel(x, y));
        }
    }
    return dst;
}

QImage ImageRotate::rotate180(QImage src) {
    QImage dst(src.width(), src.height(), src.format());
    for (int x=0;x<src.width();++x) {
        for (int y=0;y<src.height();++y) {
            dst.setPixel(src.width()-x-1, src.height()-y-1, src.pixel(x, y));
        }
    }
    return dst;
}

QImage ImageRotate::rotate270(QImage src) {
    QImage dst(src.height(), src.width(), src.format());
    for (int x=0;x<src.width();++x) {
        for (int y=0;y<src.height();++y) {
            dst.setPixel(y, src.width()-x-1, src.pixel(x, y));
        }
    }
    return dst;
}

#endif
