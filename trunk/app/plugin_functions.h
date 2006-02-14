#ifndef PLUGIN_FUNCTIONS_H
#define PLUGIN_FUNCTIONS_H

#include <cmath>

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

#endif
