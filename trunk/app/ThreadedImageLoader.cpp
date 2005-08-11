#include "ThreadedImageLoader.h"

ThreadedImageLoader::ThreadedImageLoader() {
    qRegisterMetaType<QImage>("QImage");
}

ThreadedImageLoader::~ThreadedImageLoader() {
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

void ThreadedImageLoader::run() {
    forever {
        QMutexLocker locker(&mutex);
        if (end) {
            qDebug("Thread Ends");
            return;
        }
        restart = false;
        QString f(fileName);
        QImage img(f);
        if (!restart) {
            emit imageLoaded(f, img);
        }
        if (!restart) {
            condition.wait(&mutex);
        }
    }
}

void ThreadedImageLoader::loadImage(QString fileName) {
    QMutexLocker locker(&mutex);
    this->fileName = fileName;
    end = false;
    restart = false;
    if (!isRunning()) {
        start(LowPriority);
    } else {
        restart = true;
        condition.wakeOne();
    }
}
