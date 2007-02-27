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
#ifndef QTCONCURRENT_FUTURERAGGREGATE_H
#define QTCONCURRENT_FUTURERAGGREGATE_H

#include "futureshared.h"
#include "futureresult.h"
#include "future.h"

#include <QMutex>
#include <QWaitCondition>
#include <QAtomic>
#include <QList>


namespace QtConcurrent {


/*
    ReffeType maintanis a fast non-thread safe refence count for the stored
    type and calls t->deref() when the local ref count reaches 0.
*/
template <typename T>
class ReffedType
{
public:
    ReffedType(T *obj) : object(obj), refCount(1) {}
    ReffedType(const ReffedType &other) : object(other.object), refCount(other.refCount) { ++refCount; }

    ~ReffedType() { if (--refCount == 0) { if (object) object->deref(); } }
    T *operator->() const { return object; }
    bool isNull() const { return (object == 0); }
private:
    T *object;
    int refCount;
};


template <typename ResultType>
class QFutureAggregatedResult : public FutureTypedResult<ResultType>
{
    typedef FutureTypedResult<ResultType> FutureResultType;
    typedef QMap<int, FutureResultType *> MapType;
    typedef QHash<void *, int> ReverseHash;
public:
    QFutureAggregatedResult()  { this->state = Null; };
    ~QFutureAggregatedResult();
    void addInput(FutureTypedResult<ResultType> *typedResult);
    void setInput(int index, FutureTypedResult<ResultType> *typedResult);

    void connectInput(int index, FutureTypedResult<ResultType> *typedResult);

//    void started(void *source);
    void finished(void *source);
//    void canceled(void *source);
    void resultReady(int index, void *source);
//    void progress(int progress, void *source);
//    void progressRange(int minimum, int maximum, void *source);

    ResultType waitForResult(int index = -1);
    ResultType result(int index);
    bool sendWait(int resultIndex);
    int resultCount() const;
    bool isResultReady(int index) const;
    void cancel(int index);
    bool queryState(const State state, int index = -1) const;
    
    MapType allInputs() const;
    ReffedType<FutureResultType> inputAt(int index) const;
protected:
    MapType inputs;
    ReverseHash reverseMapping;
    QSet<int> finishedInputs;
    QSet<int> readyResults;
};

template <typename ResultType>
QFutureAggregatedResult<ResultType>::~QFutureAggregatedResult()
{
    QMutexLocker lock(&this->mutex);
    foreach(FutureTypedResult<ResultType> *result, inputs)
        result->deref();
}

template <typename ResultType>
void QFutureAggregatedResult<ResultType>::addInput(FutureTypedResult<ResultType> *typedResult)
{
    QMutexLocker lock(&this->mutex);
    connectInput(inputs.count(), typedResult);
}

template <typename ResultType>
void QFutureAggregatedResult<ResultType>::setInput(int index, FutureTypedResult<ResultType> *typedResult)
{
    QMutexLocker lock(&this->mutex);
    connectInput(index, typedResult);
}

template <typename ResultType>
void QFutureAggregatedResult<ResultType>::connectInput(int index, FutureTypedResult<ResultType> *typedResult)
{
    const typename MapType::const_iterator it = inputs.find(index);
    if (it != inputs.end())
        it.value()->deref();
    inputs.insert(index, typedResult);
    reverseMapping.insert(typedResult, index);
    typedResult->ref();

    this->mutex.unlock(); // ####
    typedResult->connectOutputInterface(this);
    this->mutex.lock();
}

template <typename ResultType>
void QFutureAggregatedResult<ResultType>::finished(void *source)
{
    Q_UNUSED(source);
    this->mutex.lock();
    finishedInputs.insert(reverseMapping.value(source));

    // When all the inputs are finished, so are we.
    if (finishedInputs.count() == inputs.count())
        FutureResultBase::finished();
    else
        this->mutex.unlock();
}

template <typename ResultType>
void QFutureAggregatedResult<ResultType>::resultReady(int index, void *source)
{
    Q_UNUSED(index);
    this->mutex.lock();
    const int resultIndex = reverseMapping.value(source);
    readyResults.insert(resultIndex);
    FutureResultBase::resultReady(resultIndex);
}

template <typename ResultType>
ResultType QFutureAggregatedResult<ResultType>::waitForResult(int index)
{
    if (index == -1) {
        // wait for all.
        const MapType inputsCopy = allInputs();
        foreach(FutureResultType * input, inputsCopy) {
            input->waitForResult();
            input->deref();
        }
        index = 0;
    }

    const ReffedType<FutureResultType> input(inputAt(index));
    if (input.isNull())
        return ResultType();
    return input->waitForResult();
}

template <typename ResultType>
ResultType QFutureAggregatedResult<ResultType>::result(int index)
{
    const ReffedType<FutureResultType> input(inputAt(index));
    if (input.isNull())
        return ResultType();
    return input->result(0);
}

template <typename ResultType>
bool QFutureAggregatedResult<ResultType>::sendWait(int index)
{
    const ReffedType<FutureResultType> input(inputAt(index));
    if (input.isNull())
        return false;
    return input->sendWait(0);
}

template <typename ResultType>
int QFutureAggregatedResult<ResultType>::resultCount() const
{
    return readyResults.count();
}

template <typename ResultType>
bool QFutureAggregatedResult<ResultType>::isResultReady(int index) const
{
    return readyResults.contains(index);
}

template <typename ResultType>
void QFutureAggregatedResult<ResultType>::cancel(int index)
{
    if (index == -1) {
        // Cancel all.
        const MapType inputsCopy = allInputs();
        foreach(FutureResultType * input, inputsCopy) {
            input->cancel(-1);
            input->deref();
        }
    } else {
        // cancel result at index
        const ReffedType<FutureResultType> input = inputAt(index);
        input->cancel(-1);
    }
}

template <typename ResultType>
bool QFutureAggregatedResult<ResultType>::queryState(const State state, int index) const
{
    if (index == -1)
        return FutureResultBase::queryState(state, -1);

    ReffedType<FutureResultType> input = inputAt(index);
    if (input.isNull())
        return false;
    return input->queryState(state, -1);
}

template <typename ResultType>
typename QFutureAggregatedResult<ResultType>::MapType QFutureAggregatedResult<ResultType>::allInputs() const
{
    MapType inputsCopy;
    {
        QMutexLocker lock(&this->mutex);
        inputsCopy = inputs;
        foreach(FutureResultType * input, inputsCopy)
            input->ref();
    }

    return inputsCopy;
}

template <typename ResultType>
ReffedType<typename QFutureAggregatedResult<ResultType>::FutureResultType> QFutureAggregatedResult<ResultType>::inputAt(int index) const
{
    QMutexLocker lock(&this->mutex);
    ReffedType<FutureResultType> input(inputs.value(index));
    if (input.isNull() == false)
        input->ref();
    return input;
}


template <typename ResultType>
class QAggregatedFuture : public QFuture<ResultType>
{
    typedef QFutureAggregatedResult<ResultType> AggregatedResultHolderType;
    typedef FutureTypedResult<ResultType> TypedResultHolderType;
public:
    QAggregatedFuture();
    void addFuture(const QFuture<ResultType> &other);
    void setFuture(int index, const QFuture<ResultType> &other);
};

template <typename ResultType>
QAggregatedFuture<ResultType>::QAggregatedFuture()
:QFuture<ResultType>(new AggregatedResultHolderType())
{ }

template <typename ResultType>
void QAggregatedFuture<ResultType>::addFuture(const QFuture<ResultType> &other)
{
    static_cast<AggregatedResultHolderType *>(this->resultHolder)->addInput(
        static_cast<TypedResultHolderType *>(other.resultHolder));
}

template <typename ResultType>
void QAggregatedFuture<ResultType>::setFuture(int index, const QFuture<ResultType> &other)
{
    static_cast<AggregatedResultHolderType *>(this->resultHolder)->setInput(index,
        static_cast<TypedResultHolderType *>(other.resultHolder));
}


} // namespace QtConcurrent

#endif
