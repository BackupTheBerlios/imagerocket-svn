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
#ifndef QTCONFURREFUTURE_H
#define QTCONFURREFUTURE_H

#include <futureresult.h>
#include <futureengine.h>

#include <QDebug>
#include <QVariant>

#ifdef qdoc
class QFuture { };
#endif

namespace QtConcurrent {

class FutureResultBase;
class QFutureWatcher;
class FutureBase
{
protected:
    FutureBase(FutureResultBase * resultHolder);

    void waitForFinished();
    bool queryState(const State state, int index = -1) const;

public:
    FutureBase();
    virtual ~FutureBase();
    FutureBase(const FutureBase &other);
    FutureBase &operator=(const FutureBase &other);
    bool operator==(const FutureBase &other);

    void cancelAt(int index);
    bool isCanceledAt(int index) const { return queryState(Canceled, index); }
    void cancel() { cancelAt(-1); };
    bool isCanceled() const { return queryState(Canceled, -1); }

    void pauseAt(int index);
    bool isPausedAt(int index) const { return queryState(Paused, index); }
    void pause() { pauseAt(-1); }
    bool isPaused() { return queryState(Paused, -1); }
    
    bool isStartedAt(int index) const { return queryState(Started, index); }
    bool isStarted() const { return queryState(Started, -1); }

    bool isFinishedAt(int index) const { return queryState(Finished, index); }
    bool isFinished() const { return queryState(Finished, -1); }

    int resultCount() const;
    int progress() const;
    void wait() { return waitForFinished(); }
    friend class QFutureWatcher;
#ifndef QFUTURE_TEST
protected:
#endif
    QVariant variantResultAt(int index);
    FutureResultBase *resultHolder;
};
    
template <typename ResultType> class FutureResul;
template<> class FutureResul<void>;

template <typename ResultType>
class QAggregatedFuture;
        
template <typename ResultType = void>
class QFuture : public FutureBase
{
public:
    typedef FutureTypedResult<ResultType> ResultHolderType;
    
    QFuture() 
    : FutureBase() {}

    explicit QFuture(FutureResultBase * resultHolder)
    : FutureBase(resultHolder) { }

    explicit QFuture(const ResultType result)
     : FutureBase(new FutureFinishedResult<ResultType>(result)) { }

    ResultType result(int index = 0) const
    {
        return static_cast<ResultHolderType *>(resultHolder)->waitForResult(index);
    }
    
    operator ResultType() const 
    {
        return result(0);
    }

    QList<ResultType> results()
    {
        this->wait();
        ResultHolderType * const typedResultHolder = static_cast<ResultHolderType *>(resultHolder);
        QList<ResultType> res;
        const int resultCount = typedResultHolder->resultCount();
        for (int i = 0; i < resultCount; ++ i)
            res.append(typedResultHolder->result(i));
        return res;
    }   

    // foreach support:
    class Iterator // const_iterator
    {
    public:
        Iterator(QFuture const *future, int index) : future(future), index(index) {}
        ResultType operator*() const
        {
            return const_cast<QFuture *>(future)->result(index);
        }
        
        bool operator!=(const Iterator &other) const
        {
            if (other.index == -1)
                return (future->queryState(Running) || (index < future->resultCount()));
            
            return (index != other.index);
        }
        
        void operator++()
        {
            ++index;
        }
    private:
        QFuture const *future;
        int index;
    };

    typedef Iterator const_iterator;
    const_iterator begin() const { return Iterator(this, 0); }
    const_iterator end() const{ return Iterator(this, -1);}
    
    friend class QAggregatedFuture<ResultType>;
protected:
};

template <>
class QFuture<void> : public FutureBase
{
public:
    QFuture() 
    : FutureBase() {}

    explicit inline QFuture(FutureResultBase * resultHolder) : FutureBase(resultHolder) { }
    
    // Support copy constructing a QFuture<void> from a QFuture<T>
    template <typename ResultType>
    inline QFuture(const QFuture<ResultType> &typedFuture)
    :FutureBase(static_cast<FutureBase>(typedFuture)) {  }

    // Support assigning a QFuture<T> to a QFuture<void>
    template <typename ResultType>
    QFuture<void> &operator=(const QFuture<ResultType> &typedFuture)
    {
        static_cast<FutureBase>(*this) = static_cast<FutureBase>(typedFuture);
        return *this;
    }
    friend class QAggregatedFuture<void>;
};

} // namespace QtConcurrent

#endif
