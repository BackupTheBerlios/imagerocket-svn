/****************************************************************************
**
** Copyright (C) 2007 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Concurrent project on Trolltech Labs.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#ifndef QTCONCURRENT_QUEUE_H
#define QTCONCURRENT_QUEUE_H

#include "result.h"
#include "explicitsharing.h"
#include <QQueue>
#include <QMutex>
#include <QMutexLocker>
#include <QSharedData>

namespace QtConcurrent {

template <typename DataType>
struct QueueData : public QSharedData
{
    mutable QMutex mutex;
    QQueue<DataType> queue;
};

template <typename DataType>
class Queue
{
public:
    typedef Result<DataType> Result;
    typedef QueueData<DataType> QueueData;
    void enqueue(const DataType &item);
    Result dequeue();
    bool isEmpty() const;
private:
    QExplicitlySharedDataPointer<QueueData> d;
};

template <typename DataType>
void Queue<DataType>::enqueue(const DataType &item)
{
    QMutexLocker locker(&d->mutex);
    d->queue.enqueue(item);
}

template <typename DataType>
Result<DataType> Queue<DataType>::dequeue()
{
    QMutexLocker locker(&d->mutex);
    if (d->queue.isEmpty())
        return Result();
    else
        return Result(d->queue.dequeue());
}

template <typename DataType>
bool Queue<DataType>::isEmpty() const
{
    QMutexLocker locker(&d->mutex);
    return d->queue.isEmpty();
}

} // namespace QtConcurrent

#endif
