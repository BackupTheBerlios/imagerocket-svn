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
    changes.append(InternalImage());
    changes[0].setLocation(fileName, false);
    descriptions.append(QString());
    index = 0;
    savedIndex = 0;
    saveFormat = 0;
    saveQuality = 85;
    saveProgressive = true;
    sizeOfSavedFile = -1;
    this->fileName = fileName;
    QFileInfo f(fileName);
    shortName = f.fileName();
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

qint64 RocketImage::getSizeOfFileWhenSaved() {
    if (sizeOfSavedFile == -1) {
        QBuffer buffer;
        generateSavedFileInMemory(buffer);
        sizeOfSavedFile = buffer.size();
    }
    return sizeOfSavedFile;
}

//This is a nasty hack, but it seems to be more accurate than the QFontMetrics functions
QImage getRenderedText(const QString &text, const QFont &font,
        const QColor &color, int flags, QSize size);
QImage getRenderedText(const QString &text, const QFont &font,
        const QColor &color, int flags, QSize size) {
    size = QFontMetrics(font).boundingRect(0, 0, size.width(), size.height(), flags, text).size();
    QImage image(size, QImage::Format_ARGB32_Premultiplied);
    image.fill(qRgba(0, 0, 0, 0));
    QPainter p(&image);
    p.setFont(font);
    p.setPen(color);
    p.drawText(0, 0, size.width(), size.height(), flags, text);
    QRect rect(0, 0, size.width(), size.height());
    for (int a=0;a<image.width();++a) {
        bool empty = true;
        for (int b=0;b<size.height();++b)
            if (qAlpha(image.pixel(a, b)) > 128) {empty = false; break;}
        if (empty) rect.setLeft(a); else break;
    }
    for (int a=image.width()-1;a>=0;--a) {
        bool empty = true;
        for (int b=0;b<size.height();++b)
            if (qAlpha(image.pixel(a, b)) > 128) {empty = false; break;}
        if (empty) rect.setRight(a); else break;
    }
    for (int a=0;a<image.height();++a) {
        bool empty = true;
        for (int b=0;b<size.width();++b)
            if (qAlpha(image.pixel(b, a)) > 128) {empty = false; break;}
        if (empty) rect.setTop(a); else break;
    }
    for (int a=image.height()-1;a>=0;--a) {
        bool empty = true;
        for (int b=0;b<size.width();++b)
            if (qAlpha(image.pixel(b, a)) > 128) {empty = false; break;}
        if (empty) rect.setBottom(a); else break;
    }
    return image.copy(rect);
}

void RocketImage::renderWatermark(QImage *image, const QVariant &contents, const QFont &font,
            const QColor &color, int margin, int position) {
    QImage imgWatermark;
    if (contents.type() == QVariant::String) {
        QColor opaqueColor(color.red(), color.green(), color.blue());
        imgWatermark = getRenderedText(contents.toString(), font, opaqueColor,
                Qt::AlignLeft|Qt::AlignTop, image->size());
    } else if (contents.type() == QVariant::Image) {
        imgWatermark = contents.value< QImage >();
    } else {
        assert(0);
    }
    QSize size = imgWatermark.size();
    QPoint point;
    if (position == 1 || position == 4 || position == 7) {
        point.setX(margin);
    } else if (position == 2 || position == 5 || position == 8) {
        point.setX(image->width()/2 - size.width()/2);
    } else if (position == 3 || position == 6 || position == 9) {
        point.setX(image->width() - margin - size.width());
    } else {
        assert(0);
    }
    if (position == 1 || position == 2 || position == 3) {
        point.setY(margin);
    } else if (position == 4 || position == 5 || position == 6) {
        point.setY(image->height()/2 - size.height()/2);
    } else if (position == 7 || position == 8 || position == 9) {
        point.setY(image->height() - margin - size.height());
    } else {
        assert(0);
    }
    
    //apply alpha to image and draw it
    QImage alphaWatermark(imgWatermark.convertToFormat(QImage::Format_ARGB32_Premultiplied));
    QImage eraser(alphaWatermark.copy());
    eraser.fill(qRgba(0, 0, 0, color.alpha()));
    QPainter p(&alphaWatermark);
    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p.drawImage(0, 0, eraser);
    p.end();
    QPainter painter(image);
    painter.drawImage(point, alphaWatermark);
}

void RocketImage::renderWatermark(QImage *image) {
    QSettings settings;
    settings.beginGroup("watermark");
    if (settings.value("on").toBool()) {
        QVariant contents;
        if (settings.value("selected").toString() == "text") {
            contents = settings.value("text").toString();
        } else {
            contents = QImage(settings.value("image").toString());
        }
        RocketImage::renderWatermark(image,
                contents,
                settings.value("font").value< QFont >(),
                settings.value("color").value< QColor >(),
                settings.value("margin").toInt(),
                settings.value("position").toInt());
    }
}

void RocketImage::addChange(const QPixmap &pix, QString description) {
    ++index;
    changes.insert(index, InternalImage(pix));
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
    emit changed();
    sizeOfSavedFile = -1;
}

void RocketImage::undo() {
    assert(canUndo());
    --index;
    emit changed();
    sizeOfSavedFile = -1;
}

void RocketImage::redo() {
    assert(canRedo());
    ++index;
    emit changed();
    sizeOfSavedFile = -1;
}

void RocketImage::save(const QString &name) {
    QFileInfo info(name);
    QString ext(getSaveFormatAsText());
    QString fileName(QDir(info.path()).filePath(info.baseName()+"."+ext));
    QImage image(getPixmap().toImage());
    renderWatermark(&image);
    if (getSaveFormatAsText() == "png") {
        image.convertToFormat(QImage::Format_ARGB32).save(fileName, ext.toAscii(), saveQuality);
    } else {
        image.save(fileName, ext.toAscii(), saveQuality);
    }
    setSaved();
}

//! This saves the image into memory and gets the size.
void RocketImage::generateSavedFileInMemory(QBuffer &buffer) {
    QImageWriter writer(&buffer, getSaveFormatAsText().toAscii());
    writer.setQuality(saveQuality);
    QImage image(getPixmap().toImage());
    renderWatermark(&image);
    if (getSaveFormatAsText() == "png") {
        writer.write(image.convertToFormat(QImage::Format_ARGB32));
    } else {
        writer.write(image);
    }
    sizeOfSavedFile = buffer.size();
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
        if (getStatusIconIndex() != 0) {
            QPixmap pix(getPixmap());
            if (!pix.isNull()) {
                QPixmap thumb(pix.scaled(thumbnailSize, thumbnailSize, Qt::KeepAspectRatio));
                setThumbnailWithBackground(thumb);
            } else {
                setThumbnail(Broken);
            }
        }
    }
}

QPixmap RocketImage::getThumbnail() {
    QPixmap pix(changes[index].getThumbnail());
    if (pix.isNull()) {
        updateThumbnail();
    }
    return pix;
}

void RocketImage::updateThumbnail() {
    QPixmap pix(changes[index].getThumbnail());
    if (pix.isNull()) {
        QSettings settings;
        int thumbnailSize = settings.value("thumbnail/size", 64).toInt();
        QPixmap thumb(getPixmap().scaled(thumbnailSize, thumbnailSize, Qt::KeepAspectRatio));
        setThumbnailWithBackground(thumb);
    }
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
    changes[index].setThumbnail(thumb);
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



QList < InternalImage::SharedImage * > InternalImage::SharedImage::sharedEntries;

InternalImage::InternalImage(const QPixmap &pix) {
    shared = NULL;
    attach(new SharedImage());
    shared->pix = new QPixmap(pix);
    shared->alpha = shared->pix->hasAlphaChannel();
    shared->lastAccessed = QDateTime::currentDateTime();
}

InternalImage::InternalImage(QString location) {
    shared = NULL;
    attach(new SharedImage());
    shared->location = location;
}

InternalImage::InternalImage() {
    shared = NULL;
    attach(new SharedImage());
}
InternalImage::InternalImage(const InternalImage &original) {
    shared = NULL;
    operator=(original);
}

InternalImage::InternalImage(SharedImage *original) {
    shared = NULL;
    attach(original);
}

InternalImage &InternalImage::operator=(const InternalImage &original) {
    attach(original.shared);
    return *this;
}

InternalImage::~InternalImage() {
    detach();
}

void InternalImage::attach(SharedImage *newData) {
    detach();
    shared = newData;
    if (shared) shared->useCount++;
}

void InternalImage::detach() {
    if (shared) {
        shared->useCount--;
        if (!shared->useCount) {
            if (shared->temporaryFile) QFile(shared->location).remove();
            delete shared;
        }
        shared = NULL;
    }
}

QPixmap InternalImage::getPixmap(bool &loaded) const {
    if (!shared->pix) {
        loaded = true;
        QImage image(shared->location);
        shared->alpha = image.hasAlphaChannel();
        if (!image.isNull()) {
            shared->pix = new QPixmap(QPixmap::fromImage(image));
        }
    } else {
        loaded = false;
    }
    shared->lastAccessed = QDateTime::currentDateTime();
    return shared->pix ? *shared->pix : QPixmap();
}

bool InternalImage::freePixmap() {
    if (shared->pix) {
        if (!QFile(shared->location).exists()) {
            shared->pix->toImage().convertToFormat(QImage::Format_ARGB32)
                    .save(shared->location, "png", 100);
        }
        delete shared->pix;
        shared->pix = NULL;
        return true;
    } else {
        return false;
    }
}

void InternalImage::setLocation(QString location, bool temporary) {
    shared->location = location;
    shared->temporaryFile = temporary;
}

InternalImage::SharedImage::SharedImage() {
    sharedEntries.append(this);
    pix = NULL;
    useCount = 0;
    temporaryFile = false;
    alpha = true;
}
