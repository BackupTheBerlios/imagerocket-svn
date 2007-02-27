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
#ifndef QTCONCURRENT_FUTUREENGINE
#define QTCONCURRENT_FUTUREENGINE

#include <futureshared.h>
#include <futureresult.h>

namespace QtConcurrent {

class FutureResultBase;
template <typename T>
class QFuture;

class FutureEngineInterfaceBase : public EngineInputInterface 
{
public:
    FutureEngineInterfaceBase(FutureResultBase *resultHolder) 
    : resultHolder(resultHolder) { }

    // reporting functions available to the engine author:
    void reportProgressRange(int minimum, int maximum);
    void reportProgress(int progress);
    void setNumberOfItems(int items);
    void reportItemCmpleted(int item);
    void reportCanceled();

    bool isCanceled();
    bool isPaused();
    void pauseThread();
    int resultHolderRefCount();
protected:
    FutureResultBase *resultHolder;
};

template <typename ResultType>
class FutureEngineInterface : public FutureEngineInterfaceBase
{
public:
    FutureEngineInterface() 
    : FutureEngineInterfaceBase(new FuturePrimaryResult<ResultType>(this)) { resultHolder->refTotalOnly(); }
    QFuture<ResultType> reportStart();
    QFuture<ResultType> future();
    void reportResult(const ResultType *result, int index = -1);
    void reportFinished(const ResultType *result = 0);
    void deleteEngine();
};

template <typename ResultType>
QFuture<ResultType> FutureEngineInterface<ResultType>::reportStart()
{
    resultHolder->started(this);
    return QFuture<ResultType>(resultHolder);
}

template <typename ResultType>
QFuture<ResultType> FutureEngineInterface<ResultType>::future()
{
    return QFuture<ResultType>(resultHolder);
}

template <typename ResultType>
void FutureEngineInterface<ResultType>::reportResult(const ResultType *result, int index)
{
    static_cast<FuturePrimaryResult<ResultType> *>(resultHolder)->addResult(result, index);
}

template <typename ResultType>
void FutureEngineInterface<ResultType>::reportFinished(const ResultType *result)
{
    if (resultHolder->queryState(Finished))
        return;

    if (result)
        static_cast<FuturePrimaryResult<ResultType> *>(resultHolder)->addResult(result, -1);
    static_cast<FuturePrimaryResult<ResultType> *>(resultHolder)->finished(this);
}

template <typename ResultType>
void FutureEngineInterface<ResultType>::deleteEngine()
{
    if (resultHolder->derefTotalOnly()) {
        delete this;
    } else {
        //static_cast<FuturePrimaryResult<ResultType> *>(resultHolder)->derefEngine();
    }
}

} // namespace QtConcurrent

#endif
