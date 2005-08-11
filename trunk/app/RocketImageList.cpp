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
                generator->makeThumbnail(i->getFileName(), thumbnailSize);
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
