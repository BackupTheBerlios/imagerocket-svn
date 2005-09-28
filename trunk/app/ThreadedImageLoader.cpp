/* ThreadedImageLoader
A simple class to load images in worker threads
Copyright (C) 2005 Wesley Crossman
Email: wesley@crossmans.net

Note that this class may not be used by programs not under the GPL without permission.
Email me if you wish to discuss the use of this class in non-GPL programs.

You can redistribute and/or modify this software under the terms of the GNU
General Public License as published by the Free Software Foundation;
either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this
program; if not, write to the Free Software Foundation, Inc., 59 Temple Place,
Suite 330, Boston, MA 02111-1307 USA */

#include "ThreadedImageLoader.h"
#include <QMetaType>

/*!
   \class ThreadedImageLoader
   \short This class loads an image in its thread and emits a signal when it finishes.
    The internal thread is not stopped until the instance is deleted.
    This class was not written by someone experienced in threading, so it may have bugs.
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
    //This seems to prevent a gdb error by giving gdb time to switch to the main thread. - WJC
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
void ThreadedImageLoader::loadImage(const QString &fileName) {
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
