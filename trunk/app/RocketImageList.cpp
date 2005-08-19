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

#include "RocketImageList.h"
#include "RocketImage.h"

/*!
  \class RocketImageList
  \short This class is a smart container for images, using the #RocketImage class.
  Thumbnail generation is done within this class and #ThreadedImageLoader.
*/

RocketImageList::RocketImageList() {
    generator = new ThreadedImageLoader();
    qRegisterMetaType<QImage>("QImage");
    connect(generator, SIGNAL(imageLoaded(const QString, const QImage)),
            this, SLOT(updateThumbnail(const QString, const QImage)));
}

RocketImageList::~RocketImageList() {
    delete generator;
    foreach (RocketImage *i, list) {
        delete i;
    }
}

void RocketImageList::setLocation(QString location) {
    RocketImageList::location = location;
    QStringList imageNameFilters;
    imageNameFilters << "*.png" << "*.jpg" << "*.gif" << "*.xpm" << "*.bmp"; //TODO share this list with RocketView
    QDir dir(location);
    QStringList files = dir.entryList(
            imageNameFilters, QDir::Files|QDir::Readable, QDir::Name);
    foreach (RocketImage *i, list) {
        delete i;
    }
    list.clear();
    QVector < RocketImage * > tmp;
    foreach (QString s, files) {
        RocketImage *i = new RocketImage(dir.filePath(s));
        list.append(i);
    }
    emit listChanged();
    continueThumbnailGeneration();
    if (!list.size()) {
        QMessageBox::warning(NULL, tr("Open Folder..."),
                             tr("No images were found in the folder."));
    }
    emit indexChanged(0);
}

//! This loops through current images and checks if they need a thumbnail.
void RocketImageList::continueThumbnailGeneration() {
    bool block = false;
    QSettings settings;
    int thumbnailSize = settings.value("thumbnail/size", 64).toInt();
    int maxFileSize = settings.value("thumbnail/maxFileSize", 3000000).toInt();
    foreach (RocketImage *i, list) {
        if (!block && i->getStatusIconIndex() == RocketImage::Loading) {
            QFileInfo info(i->getFileName());
            if (info.size() > maxFileSize) {
                i->setThumbnail(RocketImage::TooLarge);
            } else {
                generator->loadImage(i->getFileName());
                block = true;
            }
        }
    }
}

QString RocketImageList::getLocation() {
    return location;
}

QString RocketImageList::getAsString(int index) {
    return list.value(index)->getFileName();
}

QPixmap RocketImageList::getThumbnail(int index) {
    return list.value(index)->getThumbnail();
}

RocketImage *RocketImageList::getAsRocketImage(int index) {
    return list.value(index);
}

void RocketImageList::setIndex(int index) {
    list[index]->setActive(true);
}

int RocketImageList::getIndex() {
    return index;
}

void RocketImageList::updateThumbnail(const QString fileName, const QImage thumbnail) {
    QSettings settings;
    int thumbnailSize = settings.value("thumbnail/size", 64).toInt();
    foreach (RocketImage *i, list) {
        bool nameMatch = (i->getFileName() == fileName);
        if (nameMatch && !thumbnail.isNull()) {
            QPixmap tmp(QPixmap::fromImage(
                    thumbnail.scaled(thumbnailSize, thumbnailSize, Qt::KeepAspectRatio)));
            i->setThumbnail(tmp);
        } else if (nameMatch) {
            i->setThumbnail(RocketImage::Broken);
        }
    }
    continueThumbnailGeneration();
}
