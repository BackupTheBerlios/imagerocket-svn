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
protected:
    QPixmap thumbnail;
    QPixmap xIcon, clickToShowIcon, loadingIcon;
    QString fileName, shortName;
    int statusIcon;
    bool transparency;
    void setSelected(bool value);
    friend void RocketImageList::setIndex(int index);
    QVector < QPixmap > changes;
    int index;
public:
    enum StatusIcon {TooLarge = 1, Broken, Loading};
    RocketImage(QString fileName);
    ~RocketImage();
    QPixmap getPixmap() {return changes[index];}
    void addChange(QPixmap pix);
    void undo();
    void redo();
    int getIndex() {return index;}
    bool canUndo() {return index > 0;}
    bool canRedo() {return index < changes.size()-1;}
    void setActive(bool value);
    bool hasTransparency() {return transparency;}
    QPixmap getThumbnail() {return thumbnail;}
    void setThumbnail(QPixmap thumb);
    void setThumbnail(StatusIcon iconType);
    QString getFileName() {return fileName;}
    QString getShortFileName() {return shortName;}
    int getStatusIconIndex() {return statusIcon;}
signals:
    void thumbnailChanged(QPixmap pix);
};

#endif
