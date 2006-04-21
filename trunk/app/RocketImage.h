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

class RocketImage : public QObject {
Q_OBJECT
public:
    enum StatusIcon {TooLarge = 1, Broken, Loading};
    RocketImage(const QString &fileName);
    ~RocketImage();
    void print(QPrinter *printer, QPainter &p);
    QPixmap getPixmap() const {return changes[index];}
    QString getDescription() const {return descriptions[index];}
    QString getDescriptionOfNext() const {
        return canRedo() ? descriptions[index+1] : QString();
    }
    int getIndex() const {return index;}
    bool canUndo() const {return index > 0;}
    bool canRedo() const {return index < changes.size()-1;}
    bool hasTransparency() const {return transparency;}
    QPixmap getThumbnail() const {return thumbnail;}
    QString getFileName() const {return fileName;}
    QString getShortFileName() const {return shortName;}
    bool operator<(const RocketImage &img) const;
    
    int getStatusIconIndex() const {return statusIcon;}
    bool isSaved() const {return savedIndex == index;}
    int getSaveFormat() const {return saveFormat;}
    QString getSaveFormatAsText() const {return saveFormat == 0 ? "jpg" : "png";}
    int getSaveQuality() const {return saveQuality;}
    bool isSaveProgressive() const {return saveProgressive;}
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
    QPixmap thumbnail;
    QPixmap xIcon, clickToShowIcon, loadingIcon, backgroundTile;
    QString fileName, shortName;
    int statusIcon, savedIndex;
    bool transparency;
    QVector < QPixmap > changes;
    QVector < QString > descriptions;
    int index;
    int saveFormat, saveQuality;
    bool saveProgressive;
    void updateThumbnail();
    void setSelected(bool value);
    friend void RocketImageList::setSelection(RocketImage *);
signals:
    void changed();
    void removeMe();
    void renamed();
};

#endif
