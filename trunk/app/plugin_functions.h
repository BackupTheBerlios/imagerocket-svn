#ifndef PLUGIN_FUNCTIONS_H
#define PLUGIN_FUNCTIONS_H

#include <cmath>
#include <cassert>
#include <QImage>

#define CLAMP(x,l,u) ((x)<(l)?(l):((x)>(u)?(u):(x)))

//The following block based on code (C) 1992-2005 Trolltech AS. All rights reserved.
//From Qt 4.1.0's qdrawhelper_p.h:
static inline uint PREMUL(uint x) {
    uint a = x >> 24;
    uint t = (x & 0xff00ff) * a;
    t = (t + ((t >> 8) & 0xff00ff) + 0x800080) >> 8;
    t &= 0xff00ff;

    x = ((x >> 8) & 0xff) * a;
    x = (x + ((x >> 8) & 0xff) + 0x80);
    x &= 0xff00;
    x |= t | (a << 24);
    return x;
}
static inline uint INV_PREMUL(uint x) {
    if (qAlpha(x) == 0) {
        return 0;
    } else {
        int a = qAlpha(x);
        return qRgba((255*qRed(x)) / a, (255*qGreen(x)) / a, (255*qBlue(x)) / a, a);
    }
}

class GammaTable {
public:
    char table[256];
    GammaTable(double gamma);
    char operator[](int index) {return table[index];}
};
GammaTable::GammaTable(double gamma) {
    for (int a=0;a<256;a++) {
        table[a] = char(std::max(0, std::min(255,
                int(std::pow(a/255.0, 1.0/gamma)*255.0 + 0.5))));;
    }
}

class ConvolutionMatrix {
private:
    int width, height;
    int *matrix;
    int offset;
public:
    ConvolutionMatrix(int width, int height);
    ConvolutionMatrix() {
        width = height = -1;
        matrix = NULL;
        offset = 0;
    }
    ~ConvolutionMatrix() {
        delete matrix;
    }
    ConvolutionMatrix(const ConvolutionMatrix &source) {
       (*this) = source;
    }
    ConvolutionMatrix &operator=(const ConvolutionMatrix &source);
    
    int get(int x, int y) const {
        return matrix[(x*height)+y];
    }
    void set(int x, int y, int value) {
        matrix[(x*height)+y] = value;
    }
    void setOffset(int newOffset) {
        offset = newOffset;
    }
    int getOffset() const {return offset;}
    int getWidth() const {return width;}
    int getSideWidth() const {return (width-1)/2;}
    int getSideHeight() const {return (height-1)/2;}
    int getHeight() const {return height;}
};
ConvolutionMatrix::ConvolutionMatrix(int width, int height) {
    assert(width > 1 && width % 2);
    assert(height > 1 && height % 2);
    this->width = width;
    this->height = height;
    offset = 0;
    matrix = new int[width*height];
    for (int a=0;a<width*height;a++) matrix[a] = 0;
}
ConvolutionMatrix &ConvolutionMatrix::operator=(const ConvolutionMatrix &source) {
    delete matrix;
    matrix = new int[source.getWidth()*source.getHeight()];
    width = source.getWidth();
    height = source.getHeight();
    offset = source.getOffset();
    //would memcpy be safe? - WJC
    for (int x=0;x<source.getWidth();x++) {
        for (int y=0;y<source.getHeight();y++) {
            set(x, y, source.get(x, y));
        }
    }
    assert(source.get(2, 1) == get(2, 1));
    return *this;
}

class ImageConvolutionMatrix {
    private:
        ConvolutionMatrix matrix;
    public:
        ImageConvolutionMatrix(ConvolutionMatrix &matrix);
        void apply(QImage &dest, QImage &nonPremultipliedSrc, int x, int y);
};
ImageConvolutionMatrix::ImageConvolutionMatrix(ConvolutionMatrix &matrix) {
    this->matrix = matrix;
}
void ImageConvolutionMatrix::apply(QImage &dest, QImage &nonPremultipliedSrc, int x, int y) {
    int startX = std::max(0, x-matrix.getSideWidth()), startY = std::max(0, y-matrix.getSideHeight());
    int endX = std::min(dest.width()-1, x+matrix.getSideWidth()), endY = std::min(dest.height()-1, y+matrix.getSideHeight());
    int count = 0, r = 0, g = 0, b = 0;
    for (int mx=startX;mx<=endX;mx++) {
        for (int my=startY;my<=endY;my++) {
            int multiplier = matrix.get(x-mx+matrix.getSideWidth(), y-my+matrix.getSideHeight());
            //qDebug("%d, %d -> %d, %d = %d", x, y, x-mx+matrix.getSideWidth(), y-my+matrix.getSideHeight(), multiplier);
            uint p = nonPremultipliedSrc.pixel(mx, my);
            count += multiplier;
            r += qRed(p) * multiplier;
            g += qGreen(p) * multiplier;
            b += qBlue(p) * multiplier;
        }
    }
    count = std::max(count, 1);
    //qDebug("%d startx %d endx %d starty %d endy %d", count, startX, endX, startY, endY);
    uint p = qRgba(CLAMP(r/count+matrix.getOffset(), 0, 255),
            CLAMP(g/count+matrix.getOffset(), 0, 255),
            CLAMP(b/count+matrix.getOffset(), 0, 255),
            qAlpha(nonPremultipliedSrc.pixel(x, y)));
    uint mp = PREMUL(p);
    dest.setPixel(x, y, mp);
}

#endif
