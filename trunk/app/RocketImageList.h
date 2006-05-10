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

#ifndef ROCKET_IMAGE_LIST
#define ROCKET_IMAGE_LIST

#include "ThreadedImageLoader.h"
class RocketImage;
#include <QtGui>

class RocketImageList : public QObject {
Q_OBJECT
protected:
    QString location;
    RocketImage *selection;
    QVector < RocketImage * > list;
    ThreadedImageLoader *generator;
    QTimer saveToDiskTimer;
    QDir tempDir, collectionTempDir;
    void continueThumbnailGeneration();
    static bool rocketImageLessThan(RocketImage *one, RocketImage *two);
    QString generateRandomString(int length = 8);
public:
    enum ListChangeType {ListReplaced, EntryCreated, EntryDeleted};
    enum SaveType {ReplaceFiles, NewLocation};
    RocketImageList();
    ~RocketImageList();
    void setLocation(QString location);
    QString getLocation();
    void saveFiles(SaveType type, QString location);
    void addImages(const QStringList &files);
    int size() {return list.size();}
    const QVector < RocketImage * > *getVector() {return &list;}
    RocketImage *first() {return list.size() ? list.first() : NULL;}
    RocketImage *previous(RocketImage *index);
    RocketImage *next(RocketImage *index);
    RocketImage *last() {return list.size() ? list.last() : NULL;}
    RocketImage *getSelection();
    void refreshImages();
public slots:
    void setSelection(RocketImage *index);
    void updateThumbnail(const QString fileName, const QImage thumbnail);
protected slots:
    void removeMeEvent();
    void renamedEvent();
    void saveToDiskTimeout();
signals:
    void selectionChanged(RocketImage *oldSelection);
    void listChanged(RocketImageList::ListChangeType type, int index = 0);
    void thumbnailChanged();
};

#endif
