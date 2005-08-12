#include "test.h"
#include <assert.h>
#include <iostream>

void Test::init(QString &fileName, lua_State *L) {
    Test::fileName = fileName;
    
    QFile script(":/test.lua");
    script.open(QFile::ReadOnly);
    QTextStream in(&script);
    QString scriptStr(in.readAll());
    if (luaL_loadbuffer(L, scriptStr.toAscii(), scriptStr.length(), "<lua>")
        || lua_pcall(L, 0, 0, 0)) {
        lua_pushfstring(L, "Error loading test.lua -- %s",
                        lua_tostring(L, -1));
        lua_error(L);
    }
}

QImage *Test::activate(QPixmap *pix) {
    QImage img(pix->toImage());
    img.detach();
    //img.invertPixels(QImage::InvertRgb);
    assert(img.depth() == 32);
    for (int y=0;y<img.height();++y) {
        uint *line = reinterpret_cast< uint * >(img.scanLine(y));
        if (img.hasAlphaChannel()) {
            for (int x=0;x<img.width();++x) {
                uint *pixel = line + x;
                *pixel = qRgba(std::min(qRed(*pixel)+20,255),
                              std::min(qGreen(*pixel)+20,255),
                              std::min(qBlue(*pixel)+20,255),
                              qAlpha(*pixel));
            }
        } else {
            for (int x=0;x<img.width();++x) {
                uint *pixel = line + x;
                *pixel = qRgb(std::min(qRed(*pixel)+20,255),
                              std::min(qGreen(*pixel)+20,255),
                              std::min(qBlue(*pixel)+20,255));
            }
        }
    }
    return new QImage(img);
}

QImage *Test::activate(QImage *img) {
    return NULL;
}

int Test::length() {
    return 1;
}

void Test::reset() {
}

QListWidgetItem *Test::createListEntry(QListWidget *parent) {
    //take the abs path of library and get the icon in the same directory
    QDir thisDir(QFileInfo(fileName).absoluteDir());
    QString name(thisDir.filePath("test.png"));
    QIcon icon(name);
    QListWidgetItem *item = new QListWidgetItem(tr("Brighten"), parent);
    item->setIcon(icon);
    item->setFlags(Qt::ItemIsEnabled);
    return item;
}

Q_EXPORT_PLUGIN(Test)
