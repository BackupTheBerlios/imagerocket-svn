#include "bc.h"
#include <cassert>
#include <algorithm>

void BrightnessContrast::init(QString &fileName, lua_State *L, QObject *parent) {
    this->fileName = fileName;
    this->parent = parent;
    
    QFile script(":/bc.lua");
    script.open(QFile::ReadOnly);
    QTextStream in(&script);
    QString scriptStr(in.readAll());
    if (luaL_loadbuffer(L, scriptStr.toAscii(), scriptStr.length(), "<lua>")
        || lua_pcall(L, 0, 0, 0)) {
        lua_pushfstring(L, "Error loading bc.lua -- %s",
                        lua_tostring(L, -1));
        lua_error(L);
    }
}

QImage *BrightnessContrast::activate(QPixmap *pix) {
    QImage img(pix->toImage());
    img.detach();
    //img.invertPixels(QImage::InvertRgb);
    assert(img.depth() == 32);
    double contrast = (100.0 + settingsToolBar->sldContrast->value()) / 100;
    contrast *= contrast;
    int brightness = settingsToolBar->sldBrightness->value();
    for (int y=0;y<img.height();++y) {
        uint *line = reinterpret_cast< uint * >(img.scanLine(y));
        for (int x=0;x<img.width();++x) {
            uint *pixel = line + x;
            int arr[3] = {qRed(*pixel), qGreen(*pixel), qBlue(*pixel)};
            for (int a=0;a<3;a++) {
                int tmp = arr[a] + brightness;
                tmp = (int)(((tmp - 255 / 2 ) * contrast) + 255 / 2);
                arr[a] = std::max(0, std::min(255, tmp));
            }
            if (img.hasAlphaChannel()) {
                *pixel = qRgba(arr[0], arr[1], arr[2], qAlpha(*pixel));
            } else {
                *pixel = qRgb(arr[0], arr[1], arr[2]);
            }
        }
    }
    return new QImage(img);
}

QImage *BrightnessContrast::activate(QImage *img) {
    return NULL;
}

QWidget *BrightnessContrast::getSettingsToolBar(QPixmap *pix) {
    if (settingsToolBar) {
        assert(0); //The settings toolbar already exists. It should be deleted first.
    }
    this->pix = pix;
    settingsToolBar = new BrightnessContrastWidget(NULL);
    connect(new QShortcut(QKeySequence("Return"), settingsToolBar->btnOk), SIGNAL(activated()),
            settingsToolBar->btnOk, SIGNAL(clicked()));
    connect(new QShortcut(QKeySequence("Enter"), settingsToolBar->btnOk), SIGNAL(activated()),
            settingsToolBar->btnOk, SIGNAL(clicked()));
    connect(new QShortcut(QKeySequence("Esc"), settingsToolBar->btnCancel),
            SIGNAL(activated()), settingsToolBar->btnCancel, SIGNAL(clicked()));
    connect(settingsToolBar->btnOk, SIGNAL(clicked()), SLOT(okClicked()));
    connect(settingsToolBar->btnCancel, SIGNAL(clicked()), SLOT(cancelClicked()));
    return settingsToolBar;
}

int BrightnessContrast::length() {
    return 1;
}

void BrightnessContrast::reset() {
}

QListWidgetItem *BrightnessContrast::createListEntry(QListWidget *parent) {
    //take the abs path of library and get the icon in the same directory
    QDir thisDir(QFileInfo(fileName).absoluteDir());
    QString name(thisDir.filePath("bc.png"));
    QIcon icon(name);
    QListWidgetItem *item = new QListWidgetItem(tr("Bright/Contrast"), parent);
    item->setIcon(icon);
    item->setFlags(Qt::ItemIsEnabled);
    return item;
}

void BrightnessContrast::okClicked() {
    AddChangeToolEvent *event = new AddChangeToolEvent;
    QImage *img = activate(pix);
    event->pixmap = new QPixmap(QPixmap::fromImage(*img));
    QCoreApplication::sendEvent(parent, event);
    settingsToolBar->deleteLater();
}

void BrightnessContrast::cancelClicked() {
    settingsToolBar->deleteLater();
}

Q_EXPORT_PLUGIN(BrightnessContrast)

