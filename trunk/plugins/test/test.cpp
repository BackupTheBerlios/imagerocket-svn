#include "test.h"
#include <cassert>
#include <algorithm>

void Test::init(QString &fileName, lua_State *L, QObject *parent) {
    this->fileName = fileName;
    this->parent = parent;
    
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

QWidget *Test::getSettingsToolBar(QPixmap *pix) {
    if (settingsToolBar) {
        assert(0); //The settings toolbar already exists. It should be deleted first.
    }
    this->pix = pix;
    QFrame *f = new QFrame();
    settingsToolBar = f;
    f->setFrameStyle(QFrame::Panel|QFrame::Raised);
    QBoxLayout *layout = new QHBoxLayout(f);
    f->setLayout(layout);
    layout->setMargin(3);
    layout->setSpacing(5);
    layout->addWidget(new QLabel("Brightness", f));
    QSlider *s = new QSlider(Qt::Horizontal, f);
    s->setMinimumSize(150, 1);
    layout->addWidget(s);
    layout->addWidget(new QLabel("Contrast", f));
    QSlider *s2 = new QSlider(Qt::Horizontal, f);
    s2->setMinimumSize(150, 1);
    layout->addWidget(s2);
    layout->addStretch(0);
    layout->addWidget(new QCheckBox("Preview", f));
    QPushButton *ok = new QPushButton("&OK", f);
    connect(new QShortcut(QKeySequence("Return"), ok), SIGNAL(activated()), ok, SIGNAL(clicked()));
    connect(ok, SIGNAL(clicked()), SLOT(okClicked()));
    layout->addWidget(ok);
    QPushButton *cancel = new QPushButton("&Cancel", f);
    connect(new QShortcut(QKeySequence("Esc"), cancel), SIGNAL(activated()), cancel, SIGNAL(clicked()));
    connect(cancel, SIGNAL(clicked()), SLOT(cancelClicked()));
    layout->addWidget(cancel);
    return settingsToolBar;
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

void Test::okClicked() {
    AddChangeToolEvent *event = new AddChangeToolEvent;
    QImage *img = activate(pix);
    event->pixmap = new QPixmap(QPixmap::fromImage(*img));
    QCoreApplication::sendEvent(parent, event);
    settingsToolBar->deleteLater();
}

void Test::cancelClicked() {
    settingsToolBar->deleteLater();
}

Q_EXPORT_PLUGIN(Test)
