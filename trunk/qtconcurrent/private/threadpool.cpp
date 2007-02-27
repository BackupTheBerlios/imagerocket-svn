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
#include "threadpool.h"
#include "stack.h"
#include <QSemaphore>
#include <QAtomic>
#include <QDebug>

namespace QtConcurrent {

Q_GLOBAL_STATIC(ThreadPool, theInstance)

/*!\internal
    QThread wrapper, provides synchronizitaion against a ThreadPool
*/
class ThreadPoolThread : public QThread
{
public:
    ThreadPoolThread(ThreadPoolPrivate *manager);
    void runTask(ThreadPoolTask *task);
    void exit();
    void run();
private:
    ThreadPoolPrivate *manager;
    ThreadPoolTask *task;
    QSemaphore taskReady;
};

/*!\internal
    ThreadPool private class.
*/
class ThreadPoolPrivate
{
public:
    typedef Stack<ThreadPoolThread *> ThreadStack;

    ThreadPoolPrivate();
    void run(ThreadPoolTask *task, QThread::Priority priority);
    void threadDone(ThreadPoolThread *thread);
    inline bool isExiting() { return exitFlag; }
    void reset();
    ThreadPoolThread * createThread();

    ThreadStack availableThreads;
    ThreadStack allThreads;
    volatile bool exitFlag;
    int expiryTimeout;
};

/*!\internal

*/
ThreadPoolThread::ThreadPoolThread(ThreadPoolPrivate *manager)
:manager(manager), task(0) { }

/*!\internal

*/
void ThreadPoolThread::runTask(ThreadPoolTask *task)
{
    Q_ASSERT(this->task == 0);
    this->task = task;
    taskReady.release();
}

/*!\internal

*/
void ThreadPoolThread::exit()
{
    taskReady.release();
}


/* \internal

*/
void ThreadPoolThread::run()
{
    forever {
        // wait for work, exiting after the expiry timeout is reached
#if QT_VERSION >= 0x040300
        if (!taskReady.tryAcquire(1, manager->expiryTimeout))
            return;
#else
        // QSemaphore timeout it not supported on Qt < 4.3.
        taskReady.acquire();
#endif

        if (task == 0)
            return;

        if (task != 0)
            task->run();

        if (manager->isExiting())
            return;

        task = 0;
        manager->threadDone(this);
    }
}

/* \internal

*/
ThreadPoolPrivate:: ThreadPoolPrivate()
    : exitFlag(false), expiryTimeout(30000)
{ }

/*! \internal

*/
void ThreadPoolPrivate::run(ThreadPoolTask *task, QThread::Priority priority)
{
    if (task == 0)
        return;

    ThreadStack::Result result = availableThreads.pop();
    ThreadPoolThread * const thread = result.isValid() ? result.data() : createThread();

    const bool setPriority = (priority != QThread::NormalPriority && priority != thread->priority());
    if (setPriority)
        thread->setPriority(priority);
    thread->runTask(task);
    thread->start();
    if (setPriority)
        thread->setPriority(QThread::NormalPriority);
}

/*! \internal
    Makes all threads exit, waits for each tread to exit and deletes it.
*/
void ThreadPoolPrivate::reset()
{
    exitFlag = true;

    ThreadStack::Result result = allThreads.pop();
    while (result.isValid()) {
        ThreadPoolThread *thread = result.data();
        thread->exit();

    //###  wait() does not wait if the thread is not started, is this a race?
    // while (thread->isRunning() == false) ;

        thread->wait();
        delete thread;
        result = allThreads.pop();
    }
}

/*! \internal

*/
ThreadPoolThread *ThreadPoolPrivate::createThread()
{
    ThreadPoolThread *newThread = new ThreadPoolThread(this);
    allThreads.push(newThread);
    return newThread;
}

/*! \internal

*/
void ThreadPoolPrivate::threadDone(ThreadPoolThread *thread)
{
    availableThreads.push(thread);
}

/*!
    \class ThreadPoolTask
    \brief is the base class for all ThreadPool work items.
*/

/*! \fn ThreadPoolTask::run()
    Implement this pure virtual function in your subclass.
*/

/*! \fn ThreadPoolTask::~ThreadPoolTask()
    ThreadPoolTask virtual destructor.
*/

/*!
    \class ThreadPool
    \brief manages a collection of QThreads.
    \threadsafe

    ThreadPool manages and recyles individual QThread objects to help
    reduce thread creation costs in programs that uses threads. Each Qt
    application has one global ThreadPool object, which can be accessed
    by calling instance().

    To use one of ThreadPool threads, subclass ThreadPoolTask and implement
    the run() virtual function. Then create an object of that class and
    pass it to ThreadPool::run().

    \code
        class HelloWorldTask : public ThreadPoolTask
        {
            void run()
            {
                qDebug() << "Hello world from thread" << QThread::currentThread();
                delete this;  // ThreadPool does not delete the tasks when they are done.
            }
        }

        HelloWorldTask *hello = new HelloWorldTask();
        ThreadPool::instance()->run(hello);
    \endcode

    Threads that are unused for a certain amount of time will
    expire. The default expiry timeout is 30000 milliseconds (30
    seconds). This can be changed using setExpiryTimeout(). Setting a
    negative expiry timeout disables the expiry mechanism.

    Note that ThreadPool is a low-level class for managing threads, see TaskManager
    or the other QtConcurrent APIs for higher level alternatives.

    \sa TaskManger
*/

/*!
    Constructs a thread pool. Use \a initialThreadCount to specify how
    many threads the thread pool should contain initially. By default,
    ThreadPool creates as many threads as there is CPU cores on the system.

    \sa QThread::idealThreadCount().
*/
ThreadPool::ThreadPool()
:d(new ThreadPoolPrivate())
{ }

/*!
    Destroyes the ThreadPool.
    This function will block until all tasks have been completed.
*/
ThreadPool::~ThreadPool()
{
    d->reset();
    delete d;
}

/*!
    Returns the global ThreadPool instance.
*/
ThreadPool *ThreadPool::instance()
{
    return theInstance();
}

/*!
    Runs the given \a task using one of the ThreadPool threads. If \a priority
    is different than QThread::NormalPriority, the thread priority will be
    temporarily changed while the task is running.

    Note that the thread pool does not take ownership of the task, which means
    that you are resposible for deleting it when it is done.
*/
void ThreadPool::run(ThreadPoolTask *task, QThread::Priority priority)
{
    d->run(task, priority);
}

/*!
    Returns the expiry timeout set by setExpiryTimeout(). The default
    expiry timeout is 30000 milliseconds (30 seconds).
*/
int ThreadPool::expiryTimeout() const
{
    return d->expiryTimeout;
}

/*!
    Sets the expiry timeout for newly created threads in the pool to
    \a expiryTimeout milliseconds.  If \a expiryTimeout is negative,
    newly created threads will not expire, i.e. they will not exit
    until the thread pool is destroyed.

    Threads that are unused for \a expiryTimeout milliseconds are
    considered to have expired and will exit. Such threads will be
    restarted as needed.

    Note that setting the expiry timeout has no effect on already
    running threads. Only newly created threads will use the new \a
    expiryTimeout. We recommend setting the expiry timeout immediately
    after creating the thread pool, before calling run().
*/
void ThreadPool::setExpiryTimeout(int expiryTimeout)
{
    if (d->expiryTimeout == expiryTimeout)
        return;
    d->expiryTimeout = expiryTimeout;
}

} // namespace QtConcurent

