#ifndef THREADED_THUMBNAIL_GENERATOR
#define THREADED_THUMBNAIL_GENERATOR

#include <QtGui>

class ThreadedThumbnailGenerator : public QThread {
Q_OBJECT
protected:
    QString fileName;
    int size;
    bool end, restart;
    QWaitCondition condition;
    QMutex mutex;
    void run();
public:
    ThreadedThumbnailGenerator();
    ~ThreadedThumbnailGenerator();
    void makeThumbnail(QString fileName, int size);
signals:
    void thumbnailGenerated(QString, QImage);
};

#endif
