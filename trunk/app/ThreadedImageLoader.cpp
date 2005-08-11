#include "ThreadedImageLoader.h"

/*!
   \class ThreadedImageLoader
   \short This class loads an image in its thread and emits a signal when it finishes.
    The internal thread is not stopped until the instance is deleted.
*/

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
        emit imageLoaded(f, img);
        if (!restart) {
            condition.wait(&mutex);
        }
    }
}

//! This loads the image given as filename. If there is a current operation, this will block while it completes.
/*! I'm not sure that, if this is called several times quickly, any call before the last is guaranteed to
    work. I'm not a threading expert. It's best not to do this anyway, since it blocks. Call it again in a
    a slot connected to #imageLoaded. -- WJC
*/

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
