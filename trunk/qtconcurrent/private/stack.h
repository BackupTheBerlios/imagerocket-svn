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
#ifndef QTCONCURRENT_STACK_H
#define QTCONCURRENT_STACK_H

#include "result.h"
#include "explicitsharing.h"
#include <QStack>
#include <QMutex>
#include <QMutexLocker>
#include <QSharedData>

namespace QtConcurrent {

template <typename DataType>
struct StackData : public QSharedData
{
    mutable QMutex mutex;
    QStack<DataType> stack;
};

/*
    Thread-safe stack that uses a mutex lock.
*/
template <typename DataType>
class Stack {
public:
    typedef Result<DataType> Result;
    typedef StackData<DataType> StackData;
    void push(const DataType &item);
    Result pop();
    bool isEmpty() const;
private:
    QExplicitlySharedDataPointer<StackData> d;
};

/*
    Push an element on the stack.
*/
template <typename DataType>
void Stack<DataType>::push(const DataType &item)
{
    QMutexLocker mutexLock(&d->mutex);
    d->stack.push(item);
}

/*
    Tries to pop an element of the stack. Returns a Result object, which
    can be queryed to see if we actually got an element. 
*/
template <typename DataType>
typename Stack<DataType>::Result Stack<DataType>::pop()
{
    QMutexLocker mutexLock(&d->mutex);
    if (d->stack.isEmpty())
        return Result();
    else
        return Result(d->stack.pop());
}

/*
    isEmpty can be unsafe in a multithreaded environment: A thread could
    pop the last element right after you call this function.
*/
template <typename DataType>
bool Stack<DataType>::isEmpty() const
{
    QMutexLocker mutexLock(&d->mutex);
    return d->stack.isEmpty();
}

} // namespace QtConcurrent

#endif
