#ifndef ROCKET_IMAGE_LIST
#define ROCKET_IMAGE_LIST

#include "ThreadedThumbnailGenerator.h"
class RocketImage;
#include <QtGui>

class RocketImageList : public QObject {
Q_OBJECT
protected:
    QString location;
    int index;
    QVector < RocketImage * > list;
    ThreadedThumbnailGenerator *generator;
    void continueThumbnailGeneration();
public:
    RocketImageList();
    ~RocketImageList();
    void setLocation(QString location);
    QString getLocation();
    unsigned int size() {return list.size();}
    const QVector < RocketImage * > *getVector() {return &list;}
    QString getAsString(int index);
    QPixmap getThumbnail(int index);
    RocketImage *getAsRocketImage(int index);
    void setIndex(int index);
    int getIndex();
public slots:
    void updateThumbnail(const QString fileName, const QImage thumbnail);
signals:
    void indexChanged(int index);
    void listChanged();
    void thumbnailChanged();
};

#endif
