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

#ifndef THREADED_IMAGE_LOADER
#define THREADED_IMAGE_LOADER

#include <QThread>
#include <QImage>
#include <QWaitCondition>
#include <QMutex>

class ThreadedImageLoader : public QThread {
Q_OBJECT
protected:
    QString fileName;
    QSize imageSize;
    Qt::AspectRatioMode aspectRatioMode;
    Qt::TransformationMode transformMode;
    bool end, restart;
    QWaitCondition condition;
    QMutex mutex;
    void run();
public:
    ThreadedImageLoader();
    ~ThreadedImageLoader();
    void loadImage(const QString &fileName, QSize imageSize = QSize(),
            Qt::AspectRatioMode aspectRatioMode = Qt::IgnoreAspectRatio,
            Qt::TransformationMode transformMode = Qt::FastTransformation);
signals:
    void imageLoaded(QString, QImage);
};

#endif
