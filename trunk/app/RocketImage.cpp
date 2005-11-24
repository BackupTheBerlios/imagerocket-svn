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

#include "RocketImage.h"
#include <cassert>

/*!
   \class RocketImage
   \short The image class which manages undo/redo, thumbnails, and image loading on a per-image basis.
*/

RocketImage::RocketImage(const QString &fileName) {
    changes.append(QPixmap());
    index = 0;
    savedIndex = 0;
    saveFormat = 0;
    saveQuality = 85;
    saveProgressive = true;
    this->fileName = fileName;
    QFileInfo f(fileName);
    shortName = f.fileName();
    transparency = false; //variable is only accurate if image is loaded
    QString fileTile(":/pixmaps/smallTransparentTile.png");
    if (!QPixmapCache::find(fileTile, backgroundTile)) {
        backgroundTile = QPixmap(fileTile);
        QPixmapCache::insert(fileTile, backgroundTile);
    }
    
    setThumbnail(Loading);
}

RocketImage::~RocketImage() {
}

void RocketImage::addChange(const QPixmap &pix) {
    ++index;
    changes.insert(index, pix);
    if (index < changes.size()-1) {
        changes.remove(index+1, changes.size()-index-1);
    }
    if (index == savedIndex) {
        //in-memory pixmaps will never again match the file on disk, to the program's knowledge,
        //since the saved pixmap is lost.
        savedIndex = -1;
    }
}

void RocketImage::undo() {
    assert(canUndo());
    --index;
    if (!thumbnail.isNull()) {
        //this is sloppy and should probably be replaced - WJC
        emit thumbnailChanged(thumbnail);
    }
}

void RocketImage::redo() {
    assert(canRedo());
    ++index;
    if (!thumbnail.isNull()) {
        //this is sloppy and should probably be replaced - WJC
        emit thumbnailChanged(thumbnail);
    }
}

void RocketImage::save(const QString &name) {
    QFileInfo info(name);
    QString ext(getSaveFormatAsText());
    QString fileName(QDir(info.path()).filePath(info.baseName()+"."+ext));
    getPixmap().save(fileName, ext.toAscii(), saveQuality);
    setSaved();
}

void RocketImage::setSaved() {
    savedIndex = index;
    if (!thumbnail.isNull()) {
        //this is sloppy and should probably be replaced - WJC
        emit thumbnailChanged(thumbnail);
    }
}

void RocketImage::setActive(bool value) {
    QSettings settings;
    int thumbnailSize = settings.value("thumbnail/size", 64).toInt();
    if (value) {
        QImage img(fileName);
        transparency = img.hasAlphaChannel();
        if (!img.isNull()) {
            changes[0] = QPixmap::fromImage(img);
            QPixmap thumb(getPixmap().scaled(thumbnailSize, thumbnailSize, Qt::KeepAspectRatio));
            setThumbnailWithBackground(thumb);
        } else {
            setThumbnail(Broken);
        }
    } else {
        changes[0] = QPixmap();
    }
}

void RocketImage::setThumbnail(const QPixmap &thumb) {
    thumbnail = thumb;
    statusIcon = 0;
    emit thumbnailChanged(thumb);
}

//! This does the same as setThumbnail(const QPixmap &) but adds a checkered background.
void RocketImage::setThumbnailWithBackground(const QPixmap &thumb) {
    QPixmap tmp(thumb.size());
    QPainter p(&tmp);
    p.drawTiledPixmap(0, 0, thumb.width(), thumb.height(), backgroundTile);
    p.drawPixmap(0, 0, thumb);
    p.end();
    setThumbnail(tmp);
}

void RocketImage::setThumbnail(StatusIcon iconType) {
    QSettings settings;
    int thumbnailSize = settings.value("thumbnail/size", 64).toInt();
    //"x.png" must be loaded from file, so it's code is a little different from the others.
    QString size(QString::number(thumbnailSize));
    QString fileX(":/pixmaps/x.png"), fileLoading(":/pixmaps/loading"), fileClickToShow(":/pixmaps/cts");
    fileLoading.append(size);
    fileClickToShow.append(size);
    if ( !QPixmapCache::find(fileX + size, xIcon)
                || !QPixmapCache::find(fileLoading, loadingIcon)
                || !QPixmapCache::find(fileClickToShow, clickToShowIcon) ) {
        int thumbnailWidth = thumbnailSize, thumbnailHeight = int(.75 * thumbnailSize);
        
        xIcon.load(fileX);
        xIcon = xIcon.scaled(thumbnailSize, thumbnailSize,
                Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QPixmapCache::insert(fileX + size, xIcon);
        
        QPixmap tmp;
        QFont font;
        font.setPointSize(7 + thumbnailSize / 32);
        QColor fontColor(64, 64, 64); //not quite black
        
        tmp = QPixmap(thumbnailWidth, thumbnailHeight);
        tmp.fill(QColor(0, 0, 0, 0));
        QPainter p(&tmp);
        p.setFont(font);
        p.setPen(fontColor);
        p.drawText(0, 0, thumbnailWidth, thumbnailHeight,
                Qt::AlignHCenter|Qt::AlignVCenter, tr("Loading..."));
        p.end();
        loadingIcon = tmp;
        QPixmapCache::insert(fileLoading, loadingIcon);
        
        tmp = QPixmap(thumbnailWidth, thumbnailHeight);
        tmp.fill(QColor(0, 0, 0, 0));
        p.begin(&tmp);
        p.setFont(font);
        p.setPen(fontColor);
        p.drawText(0, 0, thumbnailWidth, thumbnailHeight,
                Qt::AlignHCenter|Qt::AlignVCenter, tr("Click to\nShow"));
        p.end();
        clickToShowIcon = tmp;
        QPixmapCache::insert(fileClickToShow, clickToShowIcon);
    }
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
