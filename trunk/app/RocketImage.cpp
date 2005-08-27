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
#include <assert.h>

/*!
   \class RocketImage
   \short The image class which manages undo/redo, thumbnails, and image loading on a per-image basis.
*/

RocketImage::RocketImage(QString fileName) {
    changes.append(QPixmap());
    index = 0;
    RocketImage::fileName = fileName;
    QFileInfo f(fileName);
    shortName = f.fileName();
    transparency = false; //variable is only accurate if image is loaded
    
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
    QSettings settings;
    int thumbnailSize = settings.value("thumbnail/size", 64).toInt();
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
        QPixmap tmp;
        int thumbnailWidth = thumbnailSize, thumbnailHeight = int(.75 * thumbnailSize);
        QFont f;
        f.setPointSize(10 + thumbnailSize / 64);
        
        xIcon.load(fileX);
        QPixmap scaled(xIcon.scaled(thumbnailSize, thumbnailSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        QPixmapCache::insert(fileX + size, scaled);
        
        tmp = QPixmap(thumbnailWidth, thumbnailHeight);
        tmp.fill(QColor(0, 0, 0, 0));
        QPainter p(&tmp);
        p.setFont(f);
        p.drawText(0, 0, thumbnailWidth, thumbnailHeight,
                          Qt::AlignHCenter|Qt::AlignVCenter, tr("Loading..."));
        p.end();
        loadingIcon = tmp;
        QPixmapCache::insert(fileLoading, loadingIcon);
        
        tmp = QPixmap(thumbnailWidth, thumbnailHeight);
        tmp.fill(QColor(0, 0, 0, 0));
        p.begin(&tmp);
        p.setFont(f);
        p.setPen(QColor(64, 64, 64)); //not quite black
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
