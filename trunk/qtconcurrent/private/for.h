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
#ifndef QTCONCURRENT_FOR_H
#define QTCONCURRENT_FOR_H

#include <QAtomic>
#include <QThread>

#include "threadpool.h"
#include "future.h"
#include "threadmanager.h"
#include "median.h"
#include "3rdparty/mitcyclecounter/cycle.h"

namespace QtConcurrent {

class VoidFor
{
public:
    typedef void ResultType;
    inline void *result() { return 0; }
    inline void finish() {}
};

template <typename ForType>
class ForThread : public ThreadPoolTask
{
public:
    inline ForThread(ForType * const forObject)
    :forObject(forObject) { }

    /*
        Each worker thread spends it's life time in the run function,
        continously reserving and processing blocks of iterations.
    */
    void run()
    {
        ticks beforeUser = ticks();
        ticks afterUser = ticks();
        const int medianSize = 7;
        Median<double> controlPartElapsed(medianSize);
        Median<double> userPartElapsed(medianSize);
        int blockSize = 1;
        const int maxBlockSize = (forObject->Iterations / (ThreadManager::instance()->threadLimit() * 2));
        bool blockSizeMaxed = false;

        forever {
            if (forObject->isCanceled())
                break;

            // Atomically reserve a block of iterations for this thread.
#if QT_VERSION >= 0x040300
            const int begin = forObject->iterator.fetchAndAdd(blockSize);
#else
            int localIterator = forObject->iterator;
            forever {
                if (forObject->iterator.testAndSet(localIterator, localIterator + blockSize))
                    break;
                localIterator = forObject->iterator;
            }
            const int begin = localIterator;
#endif
            int end = qMin(begin + blockSize, forObject->end);
            if (begin >= end) {
                // No more work
                break;
            }

            if (blockSizeMaxed == false) {
                beforeUser = getticks();
                controlPartElapsed.addValue(elapsed(beforeUser, afterUser));
            }

            forObject->userStruct.runIterations(begin, end);

            if (blockSizeMaxed == false) {
                afterUser = getticks();
                userPartElapsed.addValue(elapsed(afterUser, beforeUser));
                // Adjust the block size if we are spending to much time in the for control code
                // compared with the user code.
                if (controlPartElapsed.isMedianValid()) {
                    // Target at least a 100:1 ratio between work time and control code time.
                    if (controlPartElapsed.median() * 100 > userPartElapsed.median()) {
#ifdef QTCONCURRENT_FOR_DEBUG
                        qDebug() << QThread::currentThread() << "adjusting block size" << controlPartElapsed.median() << userPartElapsed.median();
#endif
                        blockSize = qMin(blockSize * 2,  maxBlockSize);
                        if (blockSize == maxBlockSize) {
                            blockSizeMaxed = true;
                        } else {
                            controlPartElapsed.reset();
                            userPartElapsed.reset();
                        }
                    }
                }
            }

            // Report progress if progress reporting enabled.
            if (forObject->progressReportingEnabled) {
                int completed = forObject->completed;
                // Atomically increment the global completed counter.
                // ### this should really be a fetch-and-add.
                forever {
                    if (forObject->completed.testAndSet(completed, completed + end - begin))
                        break;
                    completed = forObject->completed;
                }
                forObject->reportProgress(forObject->completed);
            }

            // Try starting more threads if there are any iterations left.
            if (end != forObject->end)
                forObject->startThreads();
        }

#ifdef QTCONCURRENT_FOR_DEBUG
        qDebug() << "control part" << controlPartElapsed.median();
        qDebug() << "user part" << userPartElapsed.median();
        qDebug() << "blocksize" << blockSize;
#endif

        // The last thread to exit calls finish, this gives the for object and
        // the user struct a chance to do clean-up.
        if (forObject->threadCount.deref() == false)
            forObject->finish();

        QtConcurrent::ThreadManager::instance()->releaseThread();
        delete this;
    }

private:
    ForType *forObject;
};

template <typename UserStruct>
class For : public FutureEngineInterface<typename UserStruct::ResultType>
{
public:
    typedef For<UserStruct> ForType;

    For(int begin, int end, const UserStruct &userStruct)
        : iterator(begin), end(end), Iterations(end - begin), userStruct(userStruct),
          progressReportingEnabled(true)
    { }

    inline QFuture<typename UserStruct::ResultType> start()
    {
        const QFuture<typename UserStruct::ResultType> f = this->reportStart();
        this->reportProgressRange(0, end - iterator);
        this->reportProgress(0);

        // Start one thread unconditionally, then call startThreads to start as
        // many threads as neccasary. Ref threadCount twice, once for the thread we
        // start unconditionally, and once to make sure the the for object is not
        // deleted while startThreads() is running.
        threadCount = 2;
        QtConcurrent::ThreadManager::instance()->reserveThread();
        ThreadPool::instance()->run(new ForThread<ForType>(this));
        startThreads();

#if QT_VERSION >= 0x040300
        const int oldThreadCount = threadCount.fetchAndAdd(-1);
#else
        int localThreadCount = threadCount;
        forever {
            if (threadCount.testAndSet(localThreadCount, localThreadCount - 1))
                    break;
                localThreadCount = threadCount;
        }
        const int oldThreadCount = localThreadCount;
#endif


        // If all the worker threads have exited, we need to
        // call finish() here. (A worker thread will only call
        // finish() if it decrements the thread count to 0 at exit.)
        if (oldThreadCount == 1)
            finish();

        return f;
    }

    bool internalStartThread()
    {
        if (this->isCanceled())
            return false;

        if (iterator >= end)
            return false;

        if (QtConcurrent::ThreadManager::instance()->tryReserveThread() == false)
            return false;

        int localThreadCount = threadCount;
        forever {
            if (localThreadCount == 0) {
                // We're done, no need to start thread.
                QtConcurrent::ThreadManager::instance()->releaseThread();
                return false;
            }

            if (threadCount.testAndSet(localThreadCount, localThreadCount + 1))
                break;
            localThreadCount = threadCount;
        }
        ThreadPool::instance()->run(new ForThread<ForType>(this));
        return true;
    }


    void startThreads()
    {
        forever {
            if (internalStartThread() == false)
                return;
        }
    }

    inline void finish()
    {
        userStruct.finish();

        this->reportFinished(userStruct.result());
        this->deleteEngine();
    }
public:
    QAtomic iterator;
    const int end;
    const int Iterations;
    UserStruct userStruct;

    QAtomic threadCount;
    const bool progressReportingEnabled;
    QAtomic completed;
};

template <typename UserStruct>
inline QFuture<typename UserStruct::ResultType> runFor(int begin, int end, const UserStruct &forStruct)
{
    For<UserStruct> *forObj = new For<UserStruct>(begin, end, forStruct);
    return forObj->start();
}

} // namespace QtConcurrent

#endif
