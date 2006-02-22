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
    QPixmap getPixmap() {return changes[index];}
    QString getDescription() {return descriptions[index];}
    QString getDescriptionOfNext() {
        return canRedo() ? descriptions[index+1] : QString();
    }
    int getIndex() {return index;}
    bool canUndo() {return index > 0;}
    bool canRedo() {return index < changes.size()-1;}
    bool hasTransparency() {return transparency;}
    QPixmap getThumbnail() {return thumbnail;}
    QString getFileName() {return fileName;}
    QString getShortFileName() {return shortName;}
    int getStatusIconIndex() {return statusIcon;}
    bool isSaved() {return savedIndex == index;}
    int getSaveFormat() {return saveFormat;}
    QString getSaveFormatAsText() {return saveFormat == 0 ? "jpg" : "png";}
    int getSaveQuality() {return saveQuality;}
    bool isSaveProgressive() {return saveProgressive;}
public slots:
    void undo();
    void redo();
    
    void save(const QString &name);
    void generateSavedFileInMemory(QBuffer &buffer);
    
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
    void setSelected(bool value);
    friend void RocketImageList::setIndex(int index);
    QVector < QPixmap > changes;
    QVector < QString > descriptions;
    int index;
    int saveFormat, saveQuality;
    bool saveProgressive;
    void updateThumbnail();
signals:
    void thumbnailChanged(QPixmap pix);
};

#endif
