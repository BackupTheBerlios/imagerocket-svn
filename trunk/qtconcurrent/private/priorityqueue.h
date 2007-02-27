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
#ifndef QTCONCURRENT_PRIORITYQUEUE_H
#define QTCONCURRENT_PRIORITYQUEUE_H

#include "result.h"

#include <QMutex>
#include <QMutexLocker>
#include <QList>
#include <qalgorithms.h>

namespace QtConcurrent {

//#include <qdebug.h>
//#include <QByteArray>

template <typename T>
class QueueItem
{
public:
    inline QueueItem(const T &item, int priority)
    : item(item), priority(priority) {}

    inline bool operator==(const T &other) const
    {
        return (item == other);
    }

    T item;
    int priority;
};

template <typename T>
inline bool operator<(int otherPriority, const QueueItem<T> &item)
{
    return (otherPriority < item.priority);
}

template <typename T>
inline bool operator<(const QueueItem<T> &item, int otherPriority)
{
    return (item.priority < otherPriority);
}

template <typename T>
QDebug& operator<<(QDebug &debug, const QueueItem<T> &item)
{
    debug <<item.item <<" ["<< item.priority << "]";
    return debug;
}

template <typename T>
class PriorityQueue
{
public:
    typedef QueueItem<T> QueueItemType;
    typedef Result<T> ResultType;
    typedef Result<T> Result;
    typedef QList<QueueItemType> ListType;
    typedef typename ListType::iterator IteratorType;

    void enqueue(const T &item, int priority = 0);
    ResultType dequeue();
    bool isEmpty();

    void remove(const T &item);
    void remove(const T &item, int priority);

    void changePriority(const T &item, int newPriority);
    void changePriority(const T &item, int newPriority, int oldPriority);

    void pushFront(const T &item, int priority);
    void pushBack(const T &item, int priority);
    ResultType popFront();
    ResultType popBack();
protected:
    void changePriority(const T &item, int newPriority, IteratorType begin, IteratorType end);
    void remove(const T &item, IteratorType begin, IteratorType end);
private:
    ListType items;
    QMutex mutex;
};

template <typename T>
void PriorityQueue<T>::enqueue(const T &item, int priority)
{
    pushBack(item, priority);
}

template <typename T>
typename PriorityQueue<T>::ResultType PriorityQueue<T>::dequeue()
{
    return popFront();
}

template <typename T>
bool PriorityQueue<T>::isEmpty()
{
    return items.isEmpty();
}

template <typename T>
void PriorityQueue<T>::remove(const T &item)
{
    QMutexLocker lock(&mutex);
    remove(item, items.begin(), items.end());
}

template <typename T>
void PriorityQueue<T>::remove(const T &item, int priority)
{
    QMutexLocker lock(&mutex);
    IteratorType begin = qLowerBound(items.begin(), items.end(), priority);
    IteratorType end = qUpperBound(begin, items.end(), priority);
    remove(item, begin, end);
}

template <typename T>
void PriorityQueue<T>::changePriority(const T &item, int newPriority)
{
    QMutexLocker lock(&mutex);
    changePriority(item, newPriority, items.begin(), items.end());
}

template <typename T>
void PriorityQueue<T>::changePriority(const T &item, int newPriority, int oldPriority)
{
    QMutexLocker lock(&mutex);
    IteratorType begin = qLowerBound(items.begin(), items.end(), oldPriority);
    IteratorType end = qUpperBound(begin, items.end(), oldPriority);
    changePriority(item, newPriority, begin, end);    
}

template <typename T>
void PriorityQueue<T>::pushFront(const T &item, int priority)
{
    QMutexLocker lock(&mutex);
    IteratorType i = qUpperBound(items.begin(), items.end(),  priority);
    items.insert(i, QueueItemType(item, priority));
}

template <typename T>
void PriorityQueue<T>::pushBack(const T &item, int priority)
{
    QMutexLocker lock(&mutex);
    IteratorType i = qLowerBound(items.begin(), items.end(),  priority);
    items.insert(i, QueueItemType(item, priority));
}

template <typename T>
typename PriorityQueue<T>::ResultType PriorityQueue<T>::popFront()
{
    QMutexLocker lock(&mutex);
    if (items.isEmpty())
        return ResultType();
    return ResultType(items.takeLast().item);
}

template <typename T>
typename PriorityQueue<T>::ResultType PriorityQueue<T>::popBack()
{
    QMutexLocker lock(&mutex);
    if (items.isEmpty())
        return ResultType();
    return ResultType(items.takeFirst().item);
}

template <typename T>
void PriorityQueue<T>::changePriority(const T &item, int newPriority, IteratorType begin, IteratorType end)
{
    IteratorType i = qFind(begin, end, item);
    if (i == items.end())
        return;
    QueueItemType newItem(i->item, newPriority);
    items.erase(i);
    i = qLowerBound(items.begin(), items.end(),  newPriority);
    items.insert(i, newItem);
}

template <typename T>
void PriorityQueue<T>::remove(const T &item, IteratorType begin, IteratorType end)
{
    IteratorType i = qFind(begin, end, item);
    if (i == items.end())
        return;
    items.erase(i);
}

} // namespace QtConcurrent

#endif
