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
    static int thumbnailSize;
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
