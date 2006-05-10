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

#ifndef ROCKET_IMAGE
#define ROCKET_IMAGE

#include "RocketImageList.h"
#include <QtGui>

class InternalImage {
public:
    static int getImagesInMemoryCount() {return imagesInMemory;}
    InternalImage();
    InternalImage(const QPixmap &pix);
    InternalImage(QString location);
    InternalImage(const InternalImage &original);
    class SharedImage;
    InternalImage(SharedImage *shared);
    InternalImage &operator=(const InternalImage &original);
    ~InternalImage();
    QPixmap getPixmap(bool &loaded) const;
    QPixmap getPixmap() const {bool loaded = false; return getPixmap(loaded);}
    void setThumbnail(const QPixmap &pix) {shared->thumbnail = pix;}
    QPixmap getThumbnail() const {return shared->thumbnail;}
    bool hasTransparency() const {getPixmap(); return shared->alpha;}
    bool freePixmap();
    void setLocation(QString location, bool temporary);
    QString getLocation() {return shared->location;}
    bool isTemporary() {return shared->temporaryFile;}
    void setTemporary(bool temporary) {shared->temporaryFile = temporary;}
    bool isInMemory() {return bool(shared->pix);}
    
    class SharedImage {
    protected:
        static QList < SharedImage * > sharedEntries;
    public:
        static const QList < SharedImage * > getSharedEntries() {return sharedEntries;}
        SharedImage();
        ~SharedImage() {sharedEntries.removeAll(this); delete pix;}
        QPixmap *pix, thumbnail;
        bool alpha, temporaryFile;
        QString location;
        int useCount;
        QDateTime lastAccessed;
    };
protected:
    void attach(SharedImage *);
    void detach();
    SharedImage *shared;
    static int imagesInMemory;
};

class RocketImage : public QObject {
Q_OBJECT
public:
    enum StatusIcon {TooLarge = 1, Broken, Loading};
    static void renderWatermark(QImage *image, const QVariant &contents, const QFont &font,
            const QColor &color, int margin, int position);
    void renderWatermark(QImage *image);
    
    RocketImage(const QString &fileName);
    ~RocketImage();
    void print(QPrinter *printer, QPainter &p);
    QPixmap getPixmap() const {return changes[index].getPixmap();}
    QString getDescription() const {return descriptions[index];}
    QString getDescriptionOfNext() const {
        return canRedo() ? descriptions[index+1] : QString();
    }
    int getIndex() const {return index;}
    bool canUndo() const {return index > 0;}
    bool canRedo() const {return index < changes.size()-1;}
    bool hasTransparency() const {return changes[index].hasTransparency();}
    QPixmap getThumbnail();
    QString getFileName() const {return fileName;}
    QString getShortFileName() const {return shortName;}
    bool operator<(const RocketImage &img) const;
    
    int getStatusIconIndex() const {return statusIcon;}
    bool isSaved() const {return savedIndex == index;}
    int getSaveFormat() const {return saveFormat;}
    QString getSaveFormatAsText() const {return saveFormat == 0 ? "jpg" : "png";}
    int getSaveQuality() const {return saveQuality;}
    bool isSaveProgressive() const {return saveProgressive;}
    qint64 getSizeOfFileWhenSaved();
public slots:
    void undo();
    void redo();
    
    void save(const QString &name);
    void generateSavedFileInMemory(QBuffer &buffer);
    
    bool renameFile(QString fileName);
    bool deleteFile();
    bool guiRenameFile(QString fileName);
    bool guiDeleteFile();
    
    void addChange(const QPixmap &pix, QString description);
    void setActive(bool value);
    void setSaved();
    
    void setThumbnail(const QPixmap &thumb);
    void setThumbnail(StatusIcon iconType);
    void setThumbnailWithBackground(const QPixmap &thumb);
    
    void setSaveFormat(int format) {saveFormat = format;}
    void setSaveQuality(int value) {saveQuality = value;}
    void setSaveProgressive(bool value) {saveProgressive = value;}
protected:
    QPixmap xIcon, clickToShowIcon, loadingIcon, backgroundTile;
    QString fileName, shortName;
    int statusIcon, savedIndex;
    qint64 sizeOfSavedFile;
    QVector < InternalImage > changes;
    QVector < QString > descriptions;
    int index;
    int saveFormat, saveQuality;
    bool saveProgressive;
    void updateThumbnail();
    void setSelected(bool value);
signals:
    void changed();
    void removeMe();
    void renamed();
};

#endif
