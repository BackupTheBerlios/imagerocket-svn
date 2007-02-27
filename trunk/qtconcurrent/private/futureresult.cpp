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

#include "futureresult.h"
#include <threadmanager.h>

namespace QtConcurrent {

FutureResultBase::FutureResultBase()
    : currentProgress(-1), progressMinimum(0), progressMaximum(0), state(State(Null)), progressTimeStarted(false) { }

void FutureResultBase::ref()
{
    futureRefCount.ref();
    totalRefCount.ref();
}

bool FutureResultBase::deref()
{
    const bool wait = (futureRefCount.deref() == false);
    if (wait)
        this->wait();
    return derefTotalOnly();
}

void FutureResultBase::refTotalOnly()
{
    totalRefCount.ref();
}

bool FutureResultBase::derefTotalOnly()
{
    const bool deleteSelf = (totalRefCount.deref() == false);
    if (deleteSelf)
        delete this;

    return deleteSelf;
}

void FutureResultBase::setState(const State newState)
{
    state = newState;
}

bool FutureResultBase::queryState(const State queryState, int index) const
{
    Q_UNUSED(index);
    return (state & queryState);
}

void FutureResultBase::connectOutputInterface(EngineOutputInterface *outputInterface)
{
    mutex.lock();
    OutputConnection outputConnection(outputInterface);
    
    if (state & Started) {
        outputConnection.callOutQueue.enqueue(CallOut(CallOut::Started));
        outputConnection.callOutQueue.enqueue(CallOut(CallOut::ProgressRange));
        if (currentProgress != -1) {
            if (currentProgress != 0)
                outputConnection.callOutQueue.enqueue(CallOut(CallOut::Progress, 0));
            outputConnection.callOutQueue.enqueue(CallOut(CallOut::Progress, currentProgress));
        }
    }

    // ### TODO handle out-of-order results.
    for (int r = 0; r < 1000; ++r) // ####!
        if (isResultReady(r)) {
            outputConnection.callOutQueue.enqueue(CallOut(CallOut::ResultReady, r));
        }

    if (state & Paused)
        outputConnection.callOutQueue.enqueue(CallOut(CallOut::Paused));

    if (state & Canceled)
        outputConnection.callOutQueue.enqueue(CallOut(CallOut::Canceled));

    if (state & Finished)
        outputConnection.callOutQueue.enqueue(CallOut(CallOut::Finished));

    outputConnections.append(outputConnection);
    const QList<OutputConnection> outputConnectionsCopy = outputConnections;
    mutex.unlock();
    sendCallOuts(outputConnectionsCopy);
}

void FutureResultBase::disconnectOutputInterface(EngineOutputInterface *outputInterface)
{
    OutputConnection outputConnection;
    {
        QMutexLocker lock(&mutex);
        const int index = outputConnections.indexOf(outputInterface);
        if (index == -1)
            return;
        outputConnection = outputConnections.at(index);
        outputConnections.removeAt(index);
    }
    sendCallOuts(outputConnection);
}

bool FutureResultBase::wait(int resultIndex)
{
    if (!(state & Running))
        return false;

    const int waitIndex = (resultIndex == -1) ? INT_MAX : resultIndex;

    while ((state & Running)) {
        mutex.lock();
        const bool isReady = isResultReady(waitIndex);
        mutex.unlock();
        if (isReady)
            break;
        if (this->sendWait(resultIndex) == false)
            break;
    }

    QMutexLocker lock(&mutex);

    if (!(state & Running)) {
        return false;
    }

    bool releasedThread = false;
    while ((state & Running) && isResultReady(waitIndex) == false) {
        if (releasedThread == false && 0 /*isWorkerThread()*/) {
            releasedThread = true;
            ThreadManager::instance()->releaseThread();
        }
        waitCondition.wait(&mutex);
    }

    if (releasedThread)
        ThreadManager::instance()->reserveThread();

    return false;
}

void FutureResultBase::cancel(int resultIndex)
{
    Q_UNUSED(resultIndex);
    canceled(0);
}

void FutureResultBase::started(void *source)
{
    Q_UNUSED(source);
    this->mutex.lock();
    started();
}

void FutureResultBase::finished(void *source)
{
    Q_UNUSED(source);
    this->mutex.lock();
    finished();
}

void FutureResultBase::canceled(void *source)
{
    Q_UNUSED(source);
    this->mutex.lock();
    canceled();
}

void FutureResultBase::resultReady(int index, void *source)
{
    Q_UNUSED(source);
    this->mutex.lock();
    resultReady(index);
}

void FutureResultBase::progressRange(int minimum, int maximum, void *source)
{
    Q_UNUSED(source);
    this->mutex.lock();
    progressRange(minimum, maximum);
}

void FutureResultBase::progress(int newProgress, void *source)
{
    Q_UNUSED(source);
    if (currentProgress >= newProgress)
        return;

    this->mutex.lock();
    progress(newProgress);
}

void FutureResultBase::started()
{
    if ((state & Started) || (state & Canceled) || (state & Finished)) {
        this->mutex.unlock();
        return;
    }

    this->setState(State(Started | Running));
    sendCallOut(CallOut(CallOut::Started));
}

void FutureResultBase::finished()
{
    if ((state & Finished)) {
        this->mutex.unlock();
        return;
    }

    state = State((state & ~Running) | Finished);
    this->waitCondition.wakeAll();
    sendCallOut(CallOut(CallOut::Finished));
}

void FutureResultBase::canceled()
{
    if (state & Canceled) {
        this->mutex.unlock();
        return;
    }

    state = State(state | Canceled);
    waitCondition.wakeAll();
    sendCallOut(CallOut(CallOut::Canceled));
}

void FutureResultBase::resultReady(int index)
{
    if ((state & Canceled) || (state & Finished)) {
        this->mutex.unlock();
        return;
    }
    
    waitCondition.wakeAll();
    sendCallOut(CallOut(CallOut::ResultReady, index));
}

void FutureResultBase::progressRange(int minimum, int maximum)
{
    progressMinimum = minimum;
    progressMaximum = maximum;
    sendCallOut(CallOut(CallOut::ProgressRange));
}

void FutureResultBase::progress(int progress)
{
    if (currentProgress >= progress) {
        this->mutex.unlock();
        return;
    }

    if ((state & Canceled) || (state & Finished)) {
        this->mutex.unlock();
        return;
    }

    if (currentProgress >= progress) {
        this->mutex.unlock();
        return;
    }
    
    currentProgress = progress;

    const int maxProgressEmitsPerSecond = 25;
    if (progressTimeStarted == true && currentProgress != progressMaximum) // make sure the first and last steps are emitted.
        if (progressTime.elapsed() < (1000 / maxProgressEmitsPerSecond)) {
            this->mutex.unlock();
            return;
        }
    progressTime.start();
    progressTimeStarted = true;
    sendCallOut(CallOut(CallOut::Progress, currentProgress));
}

void FutureResultBase::sendCallOut(CallOut callOut)
{
    if (outputConnections.isEmpty()) {
        mutex.unlock();
        return;
    }

    enqueueCallOut(callOut);
    QList<OutputConnection> outputConnectionsCopy = outputConnections;
    mutex.unlock();
    sendCallOuts(outputConnectionsCopy);
}

void FutureResultBase::enqueueCallOut(CallOut callOut)
{
    foreach (OutputConnection outputConnection, outputConnections)
        outputConnection.callOutQueue.enqueue(callOut);
}

void FutureResultBase::sendCallOuts(const OutputConnection &connection)
{
    Queue<CallOut>::Result result = connection.callOutQueue.dequeue();
    while (result.isValid()) {
        const CallOut callOut = result.data();
        result = connection.callOutQueue.dequeue();
        
        switch (callOut.type) {
            case CallOut::Started:
                connection.outputInterface->started(this);
            break;
            case CallOut::Finished:                    
                connection.outputInterface->finished(this);
            break;
            case CallOut::Canceled:
                connection.outputInterface->canceled(this);
            break;
            case CallOut::Paused:
                connection.outputInterface->paused(this);
            break;
            case CallOut::ResultReady:
                connection.outputInterface->resultReady(callOut.index, this);
            break;
            case CallOut::Progress:
                connection.outputInterface->progress(callOut.index, this);
            break;
            case CallOut::ProgressRange:
                connection.outputInterface->progressRange(progressMinimum, progressMaximum, this);
            break;
            default: break;
        }
    }
}

void FutureResultBase::sendCallOuts(const QList<OutputConnection> &outputConnectionsCopy)
{
    foreach (OutputConnection outputConnection, outputConnectionsCopy)
        sendCallOuts(outputConnection);
}

} // namepsace QtConcurrent
