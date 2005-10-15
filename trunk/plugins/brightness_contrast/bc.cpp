#include "bc.h"
#include <cassert>
#include <algorithm>

void BrightnessContrast::init(QString &fileName, lua_State *L, QObject *parent) {
    this->fileName = fileName;
    this->parent = parent;
    updateTimer.setSingleShot(true);
    
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
    assert(pix);
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
    connect(settingsToolBar->chkPreview, SIGNAL(toggled(bool)), SLOT(previewToggled(bool)));
    connect(settingsToolBar->sldBrightness, SIGNAL(valueChanged(int)), SLOT(sliderValueChanged(int)));
    connect(settingsToolBar->sldContrast, SIGNAL(valueChanged(int)), SLOT(sliderValueChanged(int)));
    connect(&updateTimer, SIGNAL(timeout()), SLOT(emitUpdatedPreview()));
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
    delete img;
    QCoreApplication::sendEvent(parent, event);
    previewToggled(false);
    delete pix;
    delete settingsToolBar;
}

void BrightnessContrast::cancelClicked() {
    previewToggled(false);
    delete pix;
    delete settingsToolBar;
}

void BrightnessContrast::previewToggled(bool checked) {
    if (pix && checked) {
        emitUpdatedPreview();
    } else if (pix) {
        UpdatePreviewToolEvent *event = new UpdatePreviewToolEvent;
        QCoreApplication::sendEvent(parent, event);
        updateTimer.stop();
    }
}

void BrightnessContrast::emitUpdatedPreview() {
    if (pix && settingsToolBar->chkPreview->isChecked()) {
        UpdatePreviewToolEvent *event = new UpdatePreviewToolEvent;
        QImage *img = activate(pix);
        event->pixmap = new QPixmap(QPixmap::fromImage(*img));
        delete img;
        QCoreApplication::sendEvent(parent, event);
    }
}

void BrightnessContrast::sliderValueChanged(int value) {
    if (pix) {
        updateTimer.start(1000);
    }
}

Q_EXPORT_PLUGIN(BrightnessContrast)

