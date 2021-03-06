/* ImageTools
Various classes and functions for image processing
Copyright (C) 2005-2006 Wesley Crossman
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

#ifndef IMAGE_TOOLS_H
#define IMAGE_TOOLS_H

#include <QImage>
#include <QColor>
#include <cmath>
#include <cstdio>

namespace ImageTools {
    #include "imagerotate.h"
    
    inline int clamp(int n, int lower, int upper) {
        return (n<lower) ? lower : (n>upper) ? upper : n;
    }
    
    //The following two functions based on code (C) 1992-2005 Trolltech AS.
    //From Qt 4.1.0's qdrawhelper_p.h:
    uint premultiply(uint);
    uint decodePremultiplied(uint);
    uint premultiply(uint x) {
        //from Qt
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
    uint decodePremultiplied(uint x) {
        //from Qt
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
        bool applyToAlpha;
    public:
        ConvolutionMatrix(int width, int height);
        ConvolutionMatrix() {
            width = height = -1;
            matrix = NULL;
            applyToAlpha = true;
        }
        ~ConvolutionMatrix() {
            delete[] matrix;
        }
        ConvolutionMatrix(const ConvolutionMatrix &source) {
           (*this) = source;
        }
        ConvolutionMatrix &operator=(const ConvolutionMatrix &source);
        int get(int x, int y) const {return matrix[(x*height)+y];}
        void set(int x, int y, int value) {matrix[(x*height)+y] = value;}
        int getWidth() const {return width;}
        int getSideWidth() const {return (width-1)/2;}
        int getSideHeight() const {return (height-1)/2;}
        int getHeight() const {return height;}
        void setOffset(int newOffset) {offset = newOffset;}
        int getOffset() const {return offset;}
        void setApplyToAlpha(bool value) {applyToAlpha = value;}
        bool isApplyToAlpha() const {return applyToAlpha;}
    };
    ConvolutionMatrix::ConvolutionMatrix(int width, int height) {
        assert(width > 1 && width % 2);
        assert(height > 1 && height % 2);
        this->width = width;
        this->height = height;
        applyToAlpha = true;
        offset = 0;
        matrix = new int[width*height];
        for (int a=0;a<width*height;a++) matrix[a] = 0;
    }
    ConvolutionMatrix &ConvolutionMatrix::operator=(const ConvolutionMatrix &source) {
        width = source.getWidth();
        height = source.getHeight();
        applyToAlpha = source.isApplyToAlpha();
        delete[] matrix;
        matrix = new int[width*height];
        std::memcpy(matrix, source.matrix, width*height*sizeof(int));
        return *this;
    }
    
    class ImageConvolutionMatrix : public ConvolutionMatrix {
    public:
        ImageConvolutionMatrix(int width, int height);
        // /apply/ ignores apply To alpha -- /alphaWeightedApply/ uses it
        void apply(QImage &dest, QImage &nonPremultipliedSrc, int x, int y);
        void alphaWeightedApply(QImage &dest, QImage &nonPremultipliedSrc, int x, int y);
        ImageConvolutionMatrix &operator=(const ConvolutionMatrix &source) {
            ConvolutionMatrix::operator=(source);
            return *this;
        }
    };
    ImageConvolutionMatrix::ImageConvolutionMatrix(int width, int height) : ConvolutionMatrix(width, height) {
    }
    void ImageConvolutionMatrix::apply(QImage &dest, QImage &nonPremultipliedSrc, int x, int y) {
        int startX = std::max(0, x-getSideWidth()), startY = std::max(0, y-getSideHeight());
        int endX = std::min(dest.width()-1, x+getSideWidth());
        int endY = std::min(dest.height()-1, y+getSideHeight());
        int count = 0, r = 0, g = 0, b = 0;
        for (int my=startY;my<=endY;++my) {
            uint *line = reinterpret_cast< uint * >(nonPremultipliedSrc.scanLine(my));
            for (int mx=startX;mx<=endX;++mx) {
                int multiplier = get(x-mx+getSideWidth(), y-my+getSideHeight());
                uint p = line[mx];
                count += multiplier;
                r += qRed(p) * multiplier;
                g += qGreen(p) * multiplier;
                b += qBlue(p) * multiplier;
            }
        }
        count = std::max(count, 1);
        uint p = qRgba(
                clamp(int(round(double(r)/double(count)))+getOffset(), 0, 255),
                clamp(int(round(double(g)/double(count)))+getOffset(), 0, 255),
                clamp(int(round(double(b)/double(count)))+getOffset(), 0, 255),
                qAlpha(nonPremultipliedSrc.pixel(x, y)));
        dest.setPixel(x, y, premultiply(p));
    }
    void ImageConvolutionMatrix::alphaWeightedApply(QImage &dest, QImage &nonPremultipliedSrc, int x, int y) {
        int startX = std::max(0, x-getSideWidth()), startY = std::max(0, y-getSideHeight());
        int endX = std::min(dest.width()-1, x+getSideWidth());
        int endY = std::min(dest.height()-1, y+getSideHeight());
        int mCount = 0, a = 0, r = 0, g = 0, b = 0;
        for (int my=startY;my<=endY;++my) {
            uint *line = reinterpret_cast< uint * >(nonPremultipliedSrc.scanLine(my));
            for (int mx=startX;mx<=endX;++mx) {
                int multiplier = get(x-mx+getSideWidth(), y-my+getSideHeight());
                uint p = line[mx];
                int pa = qAlpha(p);
                mCount += multiplier;
                a += pa * multiplier;
                r += qRed(p) * pa * multiplier;
                g += qGreen(p) * pa * multiplier;
                b += qBlue(p) * pa * multiplier;
            }
        }
        int divisorA = std::max(a, 1);
        int divisorMCount = std::max(mCount, 1);
        uint p = qRgba(
                clamp(int(round(double(r)/double(divisorA)))+getOffset(), 0, 255),
                clamp(int(round(double(g)/double(divisorA)))+getOffset(), 0, 255),
                clamp(int(round(double(b)/double(divisorA)))+getOffset(), 0, 255),
                isApplyToAlpha()
                ? clamp(int(round(double(a)/double(divisorMCount))), 0, 255)
                : qAlpha(nonPremultipliedSrc.pixel(x, y)));
        dest.setPixel(x, y, premultiply(p));
    }
}

#endif
