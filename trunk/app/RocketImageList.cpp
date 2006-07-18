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
#include "consts.h"
#include <cstdlib>
#include <ctime>

/*!
  \class RocketImageList
  \short This class is a smart container for images, using the #RocketImage class.
  Thumbnail generation is done within this class and #ThreadedImageLoader.
*/

RocketImageList::RocketImageList() {
    selection = NULL;
    generator = new ThreadedImageLoader();
    srand(unsigned(time(NULL)));
    qRegisterMetaType<QImage>("QImage");
    connect(&saveToDiskTimer, SIGNAL(timeout()), SLOT(saveToDiskTimeout()));
    saveToDiskTimer.start(5000);
    connect(generator, SIGNAL(imageLoaded(const QString, const QImage)),
            this, SLOT(updateThumbnail(const QString, const QImage)));
    QDir temp = QDir::temp();
    if (!temp.exists(TEMP_DIR)) {
        bool tempDirCreated = temp.mkdir(TEMP_DIR);
        if (!tempDirCreated) {
            QMessageBox::warning(NULL, tr("ImageRocket"),
                    tr("Temporary folder \"%1\" could not be created.\nThis may cause problems.")
                    .arg(temp.filePath(TEMP_DIR)));
        }
    }
    if (temp.exists(TEMP_DIR)) tempDir = temp.filePath(TEMP_DIR);
}

RocketImageList::~RocketImageList() {
    delete generator;
    foreach (RocketImage *i, list) {
        delete i;
    }
    if (collectionTempDir != QDir() && collectionTempDir.exists()) {
        assert(tempDir.rmdir(collectionTempDir.dirName()));
    }
    QDir::temp().rmdir(TEMP_DIR); //does not remove it if files still exist inside
}

void RocketImageList::refreshImages() {
    foreach (RocketImage *i, list) {
        i->setThumbnail(RocketImage::Loading);
    }
    continueThumbnailGeneration();
}

void RocketImageList::setLocation(QString location) {
    QDir dir(location);
    RocketImageList::location = location;
    
    //delete current entries (this deletes any swapped-out undo steps as well)
    foreach (RocketImage *i, list) {
        delete i;
    }
    list.clear();
    
    //delete the old collection directory if one existed
    if (collectionTempDir != QDir() && collectionTempDir.exists()) {
        assert(tempDir.rmdir(collectionTempDir.dirName()));
    }
    
    //if location does not exist, return now.
    if (!dir.exists()) {
        RocketImageList::location = QString();
        emit listChanged(ListReplaced);
        selection = NULL;
        return;
    }
    
    //create new collection directory
    QString dirName = generateRandomString();
    assert(tempDir.mkdir(dirName));
    collectionTempDir = tempDir.filePath(dirName);
    
    //add valid images to imagelist
    QStringList imageNameFilters;
    foreach (QByteArray format, QImageReader::supportedImageFormats()) {
        imageNameFilters.append(QString("*.") + QString(format).toLower());
    }
    QStringList files = dir.entryList(
            imageNameFilters, QDir::Files|QDir::Readable, QDir::Name);
    QVector < RocketImage * > tmp;
    foreach (QString s, files) {
        RocketImage *i = new RocketImage(dir.filePath(s));
        connect(i, SIGNAL(removeMe()), SLOT(removeMeEvent()));
        connect(i, SIGNAL(renamed()), SLOT(renamedEvent()));
        list.append(i);
    }
    
    //select first image and notify listeners
    selection = first(); //the /selected/ can't be emited until after /listChanged/
    if (selection) selection->setActive(true);
    emit listChanged(ListReplaced);
    if (selection) emit selectionChanged(NULL); //now the signal can be emited
    
    continueThumbnailGeneration();
}

void RocketImageList::saveFiles(SaveType type, QString location) {
    if (type == ReplaceFiles) {
        foreach (RocketImage *i, *getVector()) {
            QFileInfo f(i->getShortFileName());
            if (!i->isSaved()) {
                i->save(i->getFileName());
            }
        }
    } else if (type == NewLocation) {
        QDir locationDir(location);
        foreach (RocketImage *i, *getVector()) {
            QFileInfo f(i->getShortFileName());
            if (!i->isSaved()) {
                i->save(locationDir.filePath(i->getShortFileName()));
            }
        }
    } else {
        assert(0);
    }
}

bool RocketImageList::isModified() {
    foreach (RocketImage *i, list) {
        if (!i->isSaved()) return true;
    }
    return false;
}

void RocketImageList::addScannedFile(const QPixmap &pixmap) {
    QString fileName;
    for (int a=0;a<10000;++a) {
        fileName = QDir(location).filePath(tr("%1scanned.png").arg(a, 4, 10, QLatin1Char('0')));
        if (!QFile(fileName).exists()) break;
     }
     pixmap.save(fileName, "PNG");
     addImages(QStringList() << fileName);
}

void RocketImageList::addImages(const QStringList &files) {
    bool wasEmpty = list.isEmpty();
    foreach (QString s, files) {
        QString newFile = QDir(location).filePath(QFileInfo(s).fileName());
        if (newFile != s) QFile(s).copy(newFile);
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
    bool alwaysMakeThumbnails = settings.value("thumbnail/alwaysMakeThumbnails").toBool();
    foreach (RocketImage *i, list) {
        if (!block && i->getStatusIconIndex() == RocketImage::Loading) {
            QFileInfo info(i->getFileName());
            if (info.size() > maxFileSize && !alwaysMakeThumbnails) {
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

void RocketImageList::saveToDiskTimeout() {
    QMap < QDateTime, InternalImage::SharedImage * > map;
    foreach (InternalImage::SharedImage *s, InternalImage::SharedImage::getSharedEntries()) {
        if (s->location.isNull()) {
            InternalImage internal(s);
            QString fileName = generateRandomString();
            internal.setLocation(collectionTempDir.filePath(fileName + ".png"), true);
            qDebug() << s->location;
        }
        if (!s->lastAccessed.isNull() && s->pix) map[s->lastAccessed] = s;
    }
    QList < QDateTime > keys(map.keys()), inverse;
    foreach (QDateTime dt, keys) {
        inverse.prepend(dt);
    }
    QSettings settings;
    int ramForImages = settings.value("image/ramForImages").toInt()*1024;
    int ramUsed = 0;
    for (int a=0;a<inverse.size();++a) {
        QSize s = map[inverse[a]]->pix->size();
        //This program is for editing photos, so 4 bytes per pixel should be close enough. - WJC
        ramUsed += s.width()*s.height()*4/1024;
        if (a > 0 && ramUsed > ramForImages) {
            InternalImage internal(map[inverse[a]]);
            internal.freePixmap();
            qDebug() << "Freed " << internal.getLocation();
        }
    }
}

//This generates an alphanumeric random string.
QString RocketImageList::generateRandomString(int length) {
    QString str;
    for (int a=0;a<length;++a) {
        int r = rand() % 36;
        if (r < 10) {
            r += 48;
        } else {
            r += 87;
        }
        str += QChar(char(r));
    }
    return str;
}
