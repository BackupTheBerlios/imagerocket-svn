#include "ThreadedThumbnailGenerator.h"

ThreadedThumbnailGenerator::ThreadedThumbnailGenerator() {
    qRegisterMetaType<QImage>("QImage");
}

ThreadedThumbnailGenerator::~ThreadedThumbnailGenerator() {
    mutex.lock();
    restart = true;
    end = true;
    condition.wakeOne();
    mutex.unlock();
    if (!wait(30000)) {
        qDebug("waited 30 seconds -- thread froze!");
        terminate();
    }
    //This seems to prevent a gdb error by giving gdb enough time to switch to the main thread.
    msleep(100);
}

void ThreadedThumbnailGenerator::run() {
    forever {
        QMutexLocker locker(&mutex);
        if (end) {
            qDebug("Thread Ends");
            return;
        }
        restart = false;
        QString f(fileName);
        QImage img(f);
        QImage scaled;
        int thumbnailSize = size;
        if (!img.isNull() && !restart) {
            //scaled = img.scaled(thumbnailSize, thumbnailSize, Qt::KeepAspectRatio);
            emit thumbnailGenerated(f, img);
        } else if (!restart) {
            emit thumbnailGenerated(f, scaled);
        }
        if (!restart) {
            condition.wait(&mutex);
        }
    }
}

void ThreadedThumbnailGenerator::makeThumbnail(QString fileName, int size) {
    QMutexLocker locker(&mutex);
    this->fileName = fileName;
    this->size = size;
    end = false;
    restart = false;
    if (!isRunning()) {
        start(LowPriority);
    } else {
        restart = true;
        condition.wakeOne();
    }
}
