/* ImageRocket
An image-editing program written for editing speed and ease of use.
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

#include "RocketImage.h"
#include <cassert>

/*!
   \class RocketImage
   \short The image class which manages undo/redo, thumbnails, and image loading on a per-image basis.
*/

RocketImage::RocketImage(const QString &fileName) {
    changes.append(QPixmap());
    descriptions.append(QString());
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

bool RocketImage::operator<(const RocketImage &img) const {
    return getShortFileName() < img.getShortFileName();
}

void RocketImage::addChange(const QPixmap &pix, QString description) {
    ++index;
    changes.insert(index, pix);
    descriptions.insert(index, description);
    if (index < changes.size()-1) {
        changes.remove(index+1, changes.size()-index-1);
        descriptions.remove(index+1, changes.size()-index-1);
    }
    if (index == savedIndex) {
        //in-memory pixmaps will never again match the file on disk, to the program's knowledge,
        //since the saved pixmap is lost.
        savedIndex = -1;
    }
    updateThumbnail();
}

void RocketImage::undo() {
    assert(canUndo());
    --index;
    updateThumbnail();
}

void RocketImage::redo() {
    assert(canRedo());
    ++index;
    updateThumbnail();
}

void RocketImage::save(const QString &name) {
    QFileInfo info(name);
    QString ext(getSaveFormatAsText());
    QString fileName(QDir(info.path()).filePath(info.baseName()+"."+ext));
    if (getSaveFormatAsText() == "png") {
        getPixmap().toImage().convertToFormat(QImage::Format_ARGB32).save(fileName, ext.toAscii(), saveQuality);
    } else {
        getPixmap().save(fileName, ext.toAscii(), saveQuality);
    }
    setSaved();
}

//! This saves the image into memory and gets the size.
void RocketImage::generateSavedFileInMemory(QBuffer &buffer) {
    QImageWriter writer(&buffer, getSaveFormatAsText().toAscii());
    writer.setQuality(saveQuality);
    QImage image(getPixmap().toImage());
    if (getSaveFormatAsText() == "png") {
        writer.write(image.convertToFormat(QImage::Format_ARGB32));
    } else {
        writer.write(image);
    }
}

bool RocketImage::renameFile(QString fileName) {
    QFile f(this->fileName);
    QFileInfo oldInfo(f);
    QString newName = oldInfo.dir().filePath(fileName);
    bool ok = false;
    //I loop just in case the rename failed since its thumbnail was being generated.
    for (int a=0;a<3;++a) {
        ok = f.rename(newName);
        if (ok) break;
        QThread::currentThread()->wait(400);
    }
    if (ok) {
        QFileInfo newInfo(newName);
        this->fileName = newName;
        this->shortName = newInfo.fileName();
        emit renamed();
        emit changed();
    }
    return ok;
}

bool RocketImage::deleteFile() {
    bool ok = false;
    //I loop just in case the delete failed since its thumbnail was being generated.
    for (int a=0;a<3;++a) {
        ok = QFile::remove(this->fileName);
        if (ok) break;
        QThread::currentThread()->wait(400);
    }
    if (ok) {
        emit removeMe();
        deleteLater();
    }
    return ok;
}

bool RocketImage::guiRenameFile(QString fileName) {
    bool ok = false;
    QFile f(this->fileName);
    QFileInfo oldInfo(f);
    QString newName = oldInfo.dir().filePath(fileName);
    QFile newFile(newName);
    if (newName != this->fileName) {
        if (newFile.exists()) {
            QMessageBox::warning(NULL, tr("Rename Failed"),
                    tr("%1 already exists.").arg(newName));
        } else {
            ok = RocketImage::renameFile(fileName);
            if (!ok) {
                QMessageBox::warning(NULL, tr("File Rename Failed"),
                        tr("%1 could not be renamed.").arg(getFileName()));
            }
        }
    }
    return ok;
}

bool RocketImage::guiDeleteFile() {
    bool ok = RocketImage::deleteFile();
    if (!ok) {
        QMessageBox::warning(NULL, tr("Deletion Failed"), tr("%1 could not be deleted.").arg(getFileName()));
    }
    return ok;
}

void RocketImage::print(QPrinter *printer, QPainter &p) {
    printer->newPage();
    bool active = !getPixmap().isNull();
    if (!active) setActive(true);
    QPixmap pix(getPixmap());
    if (!active) setActive(false);
    int scaledWidth = int(pix.width()/85.0*printer->logicalDpiX());
    int scaledHeight = int(pix.height()/85.0*printer->logicalDpiY());
    if (scaledWidth > scaledHeight && scaledWidth > printer->width()) {
        scaledHeight = int(printer->width() * double(pix.height())/pix.width());
        scaledWidth = printer->width();
    } else if (scaledHeight > scaledWidth && scaledHeight > printer->height()) {
        scaledWidth = int(printer->height() * double(pix.width())/pix.height());
        scaledHeight = printer->height();
    }
    int startX = printer->width()/2 - scaledWidth/2;
    int startY = printer->height()/2 - scaledHeight/2;
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.drawPixmap(startX, startY, scaledWidth, scaledHeight, pix);
}

void RocketImage::setSaved() {
    savedIndex = index;
    emit changed();
}

void RocketImage::setActive(bool value) {
    QSettings settings;
    int thumbnailSize = settings.value("thumbnail/size", 64).toInt();
    if (value) {
        QImage img(fileName);
        transparency = img.hasAlphaChannel();
        if (!img.isNull()) {
            changes[0] = QPixmap::fromImage(img);
            if (getStatusIconIndex() != 0) {
                QPixmap thumb(getPixmap().scaled(thumbnailSize, thumbnailSize, Qt::KeepAspectRatio));
                setThumbnailWithBackground(thumb);
            }
        } else {
            setThumbnail(Broken);
        }
    } else {
        changes[0] = QPixmap();
    }
}

void RocketImage::updateThumbnail() {
    QSettings settings;
    int thumbnailSize = settings.value("thumbnail/size", 64).toInt();
    QPixmap thumb(getPixmap().scaled(thumbnailSize, thumbnailSize, Qt::KeepAspectRatio));
    setThumbnailWithBackground(thumb);
}

//! This adds a checkered background before sending it to setThumbnail(const QPixmap &).
void RocketImage::setThumbnailWithBackground(const QPixmap &thumb) {
    QPixmap tmp(thumb.size());
    QPainter p(&tmp);
    p.drawTiledPixmap(0, 0, thumb.width(), thumb.height(), backgroundTile);
    p.drawPixmap(0, 0, thumb);
    p.end();
    setThumbnail(tmp);
}

void RocketImage::setThumbnail(const QPixmap &thumb) {
    thumbnail = thumb;
    statusIcon = 0;
    emit changed();
}

void RocketImage::setThumbnail(StatusIcon iconType) {
    QSettings settings;
    int thumbnailSize = settings.value("thumbnail/size", 64).toInt();
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
