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
#ifndef QTCONCURRENT_FUTURERESULT_H
#define QTCONCURRENT_FUTURERESULT_H

#include "futureshared.h"
#include "queue.h"

#include <QMutex>
#include <QWaitCondition>
#include <QAtomic>
#include <QList>
#include <QTime>
#include <QVariant>

namespace QtConcurrent {

/*
    RangedResultCounter keeps track of (possibly) out-of-order
    results but is optimized for a single result and in-order results.
*/
class RangedResultCounter
{
public:
    RangedResultCounter() : firstRange(0) {}

    void addIndex(int index)
    {
        if (index == firstRange) {
            ++firstRange;
            return;
        }
        ranges.insert(index);
    }
    
    bool isReady(int index) const
    {
        if (index < firstRange)
            return true;
        return ranges.contains(index);
    }

    int count() const
    {
        if (ranges.isEmpty())
            return firstRange;
        return firstRange + ranges.count();
    }

    // ### expose iterator instead of returning list.
    QList<int> allIndexes()
    {
        QList<int> results;
        for (int i = 0; i < firstRange; ++i)
            results.append(i);
        return results + ranges.toList();
    }

    int firstRange;
    QSet<int> ranges;
    //QMap<int, int> ranges; //TODO make it more clever.
};

struct CallOut
{
    CallOut(): type(Type(0)), index(-1) {}
    enum Type { Started, Finished, Canceled, Paused, Progress, ProgressRange, ResultReady};
    CallOut(Type type, int index = -1)
     : type(type), index(index) {}
    Type type;
    int index;
};

struct OutputConnection
{
    OutputConnection(EngineOutputInterface *outputInterface = 0) : outputInterface(outputInterface) { }
    bool operator==(EngineOutputInterface *otherOutputInterface)
    {
        return (outputInterface == otherOutputInterface);
    }

    bool operator==(const OutputConnection &other)
    {
        return (outputInterface == other.outputInterface);
    }


    EngineOutputInterface *outputInterface;
    mutable Queue<CallOut> callOutQueue;
};

class FutureResultBase :public EngineOutputInterface, public EngineInputInterface
{
public:
    FutureResultBase();
    virtual ~FutureResultBase() { }

    // ref functions for QFuture
    void ref();
    bool deref();
    // ref functions for FutureEngine
    void refTotalOnly();
    bool derefTotalOnly();


    void setState(const State state);
    virtual bool queryState(const State state, int index = -1) const;
    virtual QVariant variantResultAt(int index) const {  Q_UNUSED(index); return QVariant(); };
   
    void connectOutputInterface(EngineOutputInterface *outputInterface);
    void disconnectOutputInterface(EngineOutputInterface *outputInterface);
 
    // virtual functions that implements the EngineInput Interface

    bool wait(int resultIndex = -1);
    void cancel(int resultIndex = -1);

    // virtual functions that implements the EngineOutput Interface
    void started(void *source);
    void finished(void *source);
    void canceled(void *source);
    void resultReady(int index, void *source);
    void progress(int progress, void *source);
    void progressRange(int minimum, int maximum, void *source);

    // Internal functions that gets called by the above functions,
    // assumes that the mutex is locked.
    void started();
    void finished();
    void canceled();
    void resultReady(int index);
    void progress(int progress);
    void progressRange(int minimum, int maximum);

//  Internal functions. The mutex must be locked when calling these.
    virtual bool sendWait(int resultIndex) { Q_UNUSED(resultIndex); return false; }
    virtual int resultCount() const { return  0; }
    virtual bool isResultReady(int index) const { Q_UNUSED(index); return false; }
    void sendCallOut(CallOut callOut);
    void enqueueCallOut(CallOut callOut);

//  Callout functions, the mutex must be unlocked when calling these.
    void sendCallOuts(const QList<OutputConnection> &outputConnectionsCopy);
    void sendCallOuts(const OutputConnection &connection);
public:
    QAtomic futureRefCount;
    QAtomic totalRefCount; // includes the ref count for the engine.
    mutable QMutex mutex;
    QWaitCondition waitCondition;
    QList<OutputConnection> outputConnections;
    int currentProgress;
    int progressMinimum;
    int progressMaximum;
    State state;
    QTime progressTime;
    bool progressTimeStarted;
};

template <typename ResultType>
class FutureTypedResult : public FutureResultBase
{
public:
    virtual ~FutureTypedResult() { };
    virtual ResultType waitForResult(int index = -1) = 0; // ### merge with result
    virtual ResultType result(int index) = 0;
    QVariant variantResultAt(int index) const
    {
        QVariant v;
        qVariantSetValue(v, const_cast<FutureTypedResult *>(this)->waitForResult(index));// ### const cast.
        return v;
    }
};

template <>
class FutureTypedResult<void> : public FutureResultBase
{
public:
    virtual ~FutureTypedResult() {};
    virtual void waitForResult(int index = -1) = 0; // ### merge with result
    virtual void result(int index) = 0;
};

template <typename ResultType>
class FuturePrimaryResultBase : public FutureTypedResult<ResultType>
{
public:
    FuturePrimaryResultBase(EngineInputInterface *engine) // engine is not constructed yet!
    : engine(engine) {  }

    virtual bool sendWait(int resultIndex) { return this->engine->wait(resultIndex); }

/*
    bool refEngine()
    {
        forever {
            int local = engineRefCount;
            if (local == 0)
                return false;
            if (engineRefCount.testAndSet(local, local + 1))
                break;
            local = engineRefCount;
        }
        
        return true;
    }

    void derefEngine()
    {
        if (engineRefCount.deref() == false)
            delete engine;
    }
*/
    EngineInputInterface *engine;
    QAtomic engineRefCount;
};


template <typename ResultType>
class FuturePrimaryResult : public FuturePrimaryResultBase<ResultType>
{
public:
    FuturePrimaryResult(EngineInputInterface *engine)
    : FuturePrimaryResultBase<ResultType>(engine) { }

    ~FuturePrimaryResult()
     { qDeleteAll(m_result); }
    
    ResultType waitForResult(int index);
    ResultType result(int index);
    void addResult(const ResultType *result, int index = -1);

    int resultCount() const;
    bool isResultReady(int index) const;
    bool sendWait(int resultIndex);

    RangedResultCounter resultCounter;
    QVector<ResultType*> m_result;
};


template <typename ResultType>
ResultType FuturePrimaryResult<ResultType>::waitForResult(int index)
{
    this->wait(index);
    if (index == -1) // -1 means "wait for all results and return the first"
        return result(0);
    else
        return result(index);
}

template <typename ResultType>
ResultType FuturePrimaryResult<ResultType>::result(int index)
{
    if (isResultReady(index))
        return *m_result.at(index);
    else
        return ResultType(); 
}

template <typename ResultType>
int FuturePrimaryResult<ResultType>::resultCount() const
{
    return resultCounter.count();
}

template <typename ResultType>
bool FuturePrimaryResult<ResultType>::isResultReady(int index) const
{
    return resultCounter.isReady(index);
}

template <typename ResultType>
bool FuturePrimaryResult<ResultType>::sendWait(int resultIndex)
{
    return this->engine->wait(resultIndex);
}

template <typename ResultType>
void FuturePrimaryResult<ResultType>::addResult(const ResultType *result, int index)
{
    {
        QMutexLocker lock(&this->mutex);
        if (this->queryState(Canceled) || this->queryState(Finished))
            return;
        if (index == -1) {
            m_result.append(new ResultType(*result));
            index = m_result.count() -1;
        } else {
            if (index >= m_result.size())
                m_result.resize(index + 1);
            m_result[index] = new ResultType(*result);
        }
        resultCounter.addIndex(index);
    }
    this->resultReady(index, 0);
}

template<>
class FuturePrimaryResult<void> : public FuturePrimaryResultBase<void>
{
public:
    FuturePrimaryResult<void>(EngineInputInterface *const engine)
        : FuturePrimaryResultBase<void>(engine) {}

    void waitForResult(int index) { Q_UNUSED(index); wait(); }
    void result(int index) { Q_UNUSED(index); }
    void addResult(const void *result, int index = -1) { Q_UNUSED(result); Q_UNUSED(index);}

    int resultCount() const { return 0; }
    bool isResultReady(int index) const { Q_UNUSED(index); return false; }
};

/*
    FutureCanceledResult represents a cenceled future with no results.
*/
template <typename ResultType>
class FutureCanceledResult : public FutureTypedResult<ResultType> // ### inherit the interfaces instead.
{
public:
    FutureCanceledResult()
    { 
        this->started(this);
        this->canceled(this);
        this->finished(this);
    }
    ResultType waitForResult(int index = -1) { Q_UNUSED(index);  return ResultType(); }
    ResultType result(int index) { Q_UNUSED(index); return ResultType(); }
};

/*
    FutureFinishedResult represents a funished future with a single result.
*/
template <typename ResultType>
class FutureFinishedResult : public FutureTypedResult<ResultType> // ### inherit the interfaces instead.
{
public:
    FutureFinishedResult(const ResultType &result)  :m_result(result)
    { 
        this->started(this); 
        this->progressRange(0, 1, this);
        this->progress(1, this);
        this->finished(this);
    }
    ResultType waitForResult(int index = -1) { if (index > 0) return ResultType(); else return m_result;  }
    ResultType result(int index) { Q_UNUSED(index); return m_result; }
    int resultCount() const  { return 1;  }
    bool isResultReady(int index) const { return (index == 0); }
protected:
    ResultType m_result;
};

template <>
class FutureFinishedResult<void> : public FutureTypedResult<void>
{
public:
    FutureFinishedResult()
    { 
        this->started(this); 
        this->progressRange(0, 1, this);
        this->progress(1, this);
        this->finished(this);
    }
    void waitForResult(int index = -1)  { Q_UNUSED(index); }
    void result(int index) { Q_UNUSED(index); }
    int resultCount() const { return 0; }
    bool isResultReady(int index) const  { Q_UNUSED(index); return false; }
};

} // namespace QtConcurrent

#endif
