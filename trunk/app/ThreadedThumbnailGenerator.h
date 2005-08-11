#ifndef THREADED_IMAGE_LOADER
#define THREADED_IMAGE_LOADER

#include <QtGui>

class ThreadedImageLoader : public QThread {
Q_OBJECT
protected:
    QString fileName;
    int size;
    bool end, restart;
    QWaitCondition condition;
    QMutex mutex;
    void run();
public:
    ThreadedImageLoader();
    ~ThreadedImageLoader();
    void makeThumbnail(QString fileName, int size);
signals:
    void imageLoaded(QString, QImage);
};

#endif
