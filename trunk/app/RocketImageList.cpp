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

#include "RocketImageList.h"
#include "RocketImage.h"

/*!
  \class RocketImageList
  \short This class is a smart container for images, using the #RocketImage class.
  Thumbnail generation is done within this class and #ThreadedImageLoader.
*/

RocketImageList::RocketImageList() {
    selection = NULL;
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

void RocketImageList::refreshImages() {
    foreach (RocketImage *i, list) {
        i->setThumbnail(RocketImage::Loading);
    }
    continueThumbnailGeneration();
}

void RocketImageList::setLocation(QString location) {
    QStringList imageNameFilters;
    foreach (QByteArray format, QImageReader::supportedImageFormats()) {
        imageNameFilters.append(QString("*.") + QString(format).toLower());
    }
    QDir dir(location);
    if (!dir.exists()) {
        RocketImageList::location = QString();
        return;
    }
    RocketImageList::location = location;
    QStringList files = dir.entryList(
            imageNameFilters, QDir::Files|QDir::Readable, QDir::Name);
    foreach (RocketImage *i, list) {
        delete i;
    }
    list.clear();
    QVector < RocketImage * > tmp;
    foreach (QString s, files) {
        RocketImage *i = new RocketImage(dir.filePath(s));
        connect(i, SIGNAL(removeMe()), SLOT(removeMeEvent()));
        connect(i, SIGNAL(renamed()), SLOT(renamedEvent()));
        list.append(i);
    }
    selection = first(); //the /selected/ can't be emited until after /listChanged/
    if (selection) selection->setActive(true);
    emit listChanged(ListReplaced);
    if (selection) emit selectionChanged(NULL); //now the signal can be emited
    
    continueThumbnailGeneration();
}

void RocketImageList::addImages(const QStringList &files) {
    bool wasEmpty = list.isEmpty();
    foreach (QString s, files) {
        QString newFile = QDir(location).filePath(QFileInfo(s).fileName());
        QFile(s).copy(newFile);
        RocketImage *i = new RocketImage(newFile);
        connect(i, SIGNAL(removeMe()), SLOT(removeMeEvent()));
        connect(i, SIGNAL(renamed()), SLOT(renamedEvent()));
        list.append(i);
    }
    qSort(list.begin(), list.end(), &rocketImageLessThan);
    if (wasEmpty) {
        selection = first();
        selection->setActive(true);
    }
    emit listChanged(EntryCreated);
    if (wasEmpty) {
        emit selectionChanged(NULL);
    }
    continueThumbnailGeneration();
}

//! This loops through current images and checks if they need a thumbnail.
void RocketImageList::continueThumbnailGeneration() {
    bool block = false;
    QSettings settings;
    int maxFileSize = settings.value("thumbnail/maxFileSize", 3000000).toInt();
    int thumbnailSize = settings.value("thumbnail/size", 64).toInt();
    foreach (RocketImage *i, list) {
        if (!block && i->getStatusIconIndex() == RocketImage::Loading) {
            QFileInfo info(i->getFileName());
            if (info.size() > maxFileSize) {
                i->setThumbnail(RocketImage::TooLarge);
            } else {
                generator->loadImage(i->getFileName(), QSize(thumbnailSize, thumbnailSize),
                        Qt::KeepAspectRatio);
                block = true;
            }
        }
    }
}

QString RocketImageList::getLocation() {
    return location;
}

void RocketImageList::setSelection(RocketImage *newSelection) {
    if (selection) {
        selection->setActive(false);
    }
    if (newSelection) {
        newSelection->setActive(true);
    }
    RocketImage *oldSelection = selection;
    selection = newSelection;
    emit selectionChanged(oldSelection);
}

RocketImage *RocketImageList::getSelection() {
    return selection;
}

RocketImage *RocketImageList::previous(RocketImage *index) {
    int i = list.indexOf(index);
    return (i > 0) ? list[i-1] : NULL;
}

RocketImage *RocketImageList::next(RocketImage *index) {
    int i = list.indexOf(index);
    return (i > -1 && i < list.size()-1) ? list[i+1] : NULL;
}

void RocketImageList::updateThumbnail(const QString fileName, const QImage thumbnail) {
    foreach (RocketImage *i, list) {
        bool nameMatch = (i->getFileName() == fileName);
        if (nameMatch && !thumbnail.isNull()) {
            if (i->getStatusIconIndex() == RocketImage::Loading) {
                i->setThumbnailWithBackground(QPixmap::fromImage(thumbnail));
            }
        } else if (nameMatch) {
            i->setThumbnail(RocketImage::Broken);
        }
    }
    continueThumbnailGeneration();
}

void RocketImageList::removeMeEvent() {
    RocketImage *img = dynamic_cast < RocketImage * >(sender());
    RocketImage *newSel = next(img) ? next(img) : previous(img);
    int index = list.indexOf(img);
    list.remove(index);
    if (img == selection) setSelection(newSel);
    emit listChanged(EntryDeleted, index);
}

bool RocketImageList::rocketImageLessThan(RocketImage *one, RocketImage *two) {
    return *one < *two;
}

void RocketImageList::renamedEvent() {
    RocketImage *img = dynamic_cast < RocketImage * >(sender());
    qSort(list.begin(), list.end(), &rocketImageLessThan);
    emit listChanged(ListReplaced);
}
