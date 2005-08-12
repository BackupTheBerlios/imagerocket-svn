#include "RocketImage.h"
#include <assert.h>

int RocketImage::thumbnailSize = 0;

/*!
   \class RocketImage
   \short The image class which manages undo/redo, thumbnails, and image loading on a per-image basis.
*/

RocketImage::RocketImage(QString fileName) {
    changes.append(QPixmap());
    index = 0;
    if (!thumbnailSize) {
        QSettings settings;
        thumbnailSize = settings.value("thumbnail/size", 64).toInt();
    }
    int thumbnailWidth = thumbnailSize, thumbnailHeight = int(.75 * thumbnailSize);
    RocketImage::fileName = fileName;
    QFileInfo f(fileName);
    shortName = f.fileName();
    transparency = false; //variable is only accurate if image is loaded
    
    QString file(":/pixmaps/trash.png");
    QString file2(":/pixmaps/trashLit.png");
    QString file3(":/pixmaps/x.png");
    if (!QPixmapCache::find(file3, xIcon)
         || !QPixmapCache::find(":/pixmaps/loading.png", loadingIcon)) {
        xIcon.load(file3);
        QPixmapCache::insert(file3, xIcon);
        
        QPixmap tmp(thumbnailWidth, thumbnailHeight);
        tmp.fill(QColor(0, 0, 0, 0));
        QPainter tmpPaint(&tmp);
        tmpPaint.drawText(0, 0, thumbnailWidth, thumbnailHeight,
                Qt::AlignHCenter|Qt::AlignVCenter, tr("Loading..."));
        tmpPaint.end();
        loadingIcon = tmp;
        QPixmapCache::insert(file3, loadingIcon);
    }
    //TODO cache click-to-show icon
    QPixmap tmp(thumbnailWidth, thumbnailHeight);
    tmp.fill(QColor(0, 0, 0, 0));
    QPainter p(&tmp);
    p.setPen(QColor(64, 64, 64)); //not quite black
    p.drawText(0, 0, thumbnailWidth, thumbnailHeight,
               Qt::AlignHCenter|Qt::AlignVCenter, tr("Click to\nShow"));
    p.end();
    clickToShowIcon = tmp;
    
    setThumbnail(Loading);
}

RocketImage::~RocketImage() {
}

void RocketImage::addChange(QPixmap pix) {
    changes.insert(index+1, pix);
    index = index+1;
    if (index+1<changes.size()-1) {
        changes.remove(index+1, changes.size()-index-1);
    }
}

void RocketImage::undo() {
    assert(canUndo());
    index--;
}

void RocketImage::redo() {
    assert(canRedo());
    ++index;
}

void RocketImage::setActive(bool value) {
    if (value) {
        QImage img(fileName);
        transparency = img.hasAlphaChannel();
        if (!img.isNull()) {
            changes[0] = QPixmap::fromImage(img);
            QPixmap thumb(getPixmap().scaled(thumbnailSize, thumbnailSize, Qt::KeepAspectRatio));
            setThumbnail(thumb);
        } else {
            setThumbnail(Broken);
        }
    } else {
        changes[0] = QPixmap();
    }
}

void RocketImage::setThumbnail(QPixmap thumb) {
    thumbnail = thumb;
    statusIcon = 0;
    emit thumbnailChanged(thumb);
}

void RocketImage::setThumbnail(StatusIcon iconType) {
    switch (iconType) {
    case Broken:
        setThumbnail(xIcon);
        break;
    case TooLarge:
        setThumbnail(clickToShowIcon);
        break;
    case Loading:
        setThumbnail(loadingIcon);
        break;
    default:
        assert(0);
        break;
    }
    statusIcon = iconType;
}
