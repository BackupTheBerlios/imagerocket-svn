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
#include "future.h"

/*! \class QFuture
    \threadsafe
    \brief The QFuture class represents the result of an asynchronous computation.

    QFuture allows synchronizing a thread against one or more results which will be
    ready at a later point in time. The result can be any type that has a default 
    constructor and a copy constructor. QFuture also offers ways of interacting 
    with a runnning computation, such as querying for progress or cancelling it. 
    QFuture is a lightweight reference counted class that can be passed by value.

    To interact with running tasks using signals and slots, use QFutureWatcher.
    
    \sa QFutureWatcher
*/

namespace QtConcurrent {

FutureBase::FutureBase()
: resultHolder(new FutureCanceledResult<void>())
{
    resultHolder->ref();
}

FutureBase::FutureBase(FutureResultBase * resultHolder)
 : resultHolder(resultHolder)
 {
    resultHolder->ref();
 }
 
FutureBase::~FutureBase()
{
    resultHolder->deref();
}

FutureBase::FutureBase(const FutureBase &other)
 : resultHolder(other.resultHolder)
{
    resultHolder->ref();
}

FutureBase& FutureBase::operator=(const FutureBase &other)
{
    if (this == &other)
        return *this;
    resultHolder->deref();
    resultHolder = other.resultHolder;
    resultHolder->ref();
    return *this;
}

bool FutureBase::operator==(const FutureBase &other)
{
    return (resultHolder == other.resultHolder);
}

void FutureBase::waitForFinished()
{
    resultHolder->wait();
}

bool FutureBase::queryState(const State state, int index) const
{
    return resultHolder->queryState(state, index);
}

/*
    Cancels the task in progress.
    
    QFutureWatchers associated with this future will emit canceled 
    before this function returns. Depending on the task implementation,
    threads may contiunue to to work on the task after this function
    returns, but no results will be reported and no signals will be 
    emitted. 
*/
void FutureBase::cancelAt(int index)
{
    resultHolder->cancel(index);
}

int FutureBase::resultCount() const
{
    return resultHolder->resultCount();
}

int FutureBase::progress() const
{ 
    return resultHolder->currentProgress; // ####
} 


QVariant FutureBase::variantResultAt(int index)
{
    return resultHolder->variantResultAt(index);
}


} // namepsace QtConcurrent
