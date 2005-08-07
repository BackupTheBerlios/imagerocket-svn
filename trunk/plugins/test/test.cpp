#include "test.h"
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
    img.invertPixels(QImage::InvertRgb);
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
    QString name(QDir(QFileInfo(fileName).absoluteDir()).filePath("test.png"));
    QIcon icon(name);
    QListWidgetItem *item = new QListWidgetItem(tr("Invert"), parent);
    item->setIcon(icon);
    item->setFlags(Qt::ItemIsEnabled);
    return item;
}

Q_EXPORT_PLUGIN(Test)
