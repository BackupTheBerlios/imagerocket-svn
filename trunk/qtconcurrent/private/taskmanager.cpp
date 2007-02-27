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
#include "taskmanager.h"
#include <private/qobject_p.h>

#include <result.h>
#include <QMutex>
#include <QWaitCondition>

namespace QtConcurrent {

Q_GLOBAL_STATIC(TaskManager, globalTaskManager)

// A Semaphore that can wait until all resources are returned.
class ReverseSemaphore
{
public:
    ReverseSemaphore()
    :count(0) { }

    ReverseSemaphore(int initialCount)
    :count(initialCount) { }

    void acquire()
    {
        QMutexLocker lock(&mutex);
        ++count;
    }
    
    int release()
    {
        QMutexLocker lock(&mutex);
        if (--count == 0)
            waitCondition.wakeAll();
        return count;
    }

    // release one resource and wait until all resources are released.
    void wait()
    {
        QMutexLocker lock(&mutex);
        if (count != 0)
            waitCondition.wait(&mutex);
    }
    
    int currentCount()
    {
        return count;
    }

private:
    int count;
    QMutex mutex;
    QWaitCondition waitCondition;
};


/////////////////////////////////////////////////////////////////////////////////////////////////

class TaskManagerPrivate : public QObjectPrivate
{
public:
    TaskManagerPrivate()
    : threadManager(ThreadManager::instance()), threadPool(ThreadPool::instance()), isExiting(false)
    { }

    QReadWriteLock listLock;
    ReverseSemaphore taskRunners;
    TaskManager::QueueType taskQueue;
    ThreadManager *threadManager;
    ThreadPool *threadPool;
    bool isExiting;
};

class TaskRunner: public ThreadPoolTask
{
public:
    typedef TaskManager TaskManagerType;

    TaskRunner(TaskManager *taskManager)
        : taskManager(taskManager) { }

    void run();
    TaskManager *taskManager;
};



/*!
    \class QtConcurrent::TaskManager
    \brief allows running tasks or functions asynchronously in a different thread.
    \threadsafe
    
    running functions
    run returns a QFuture that contains the reusult
    defining tasks, running tasks
    thread safety of tasks and functions
    the queue
    the thread manager
    the thread pool
*/

TaskManager::TaskManager(QObject *parent)
 : QObject(*new TaskManagerPrivate(), parent)
 { }

TaskManager::~TaskManager()
{
    Q_D(TaskManager);
    d->isExiting = true;

    // wait until all taskRunners have finished.
    d->taskRunners.wait(); 
}

/*!
    Returns the global TaskManager.
*/
TaskManager *TaskManager::instance()
{
    return globalTaskManager();
}

/*!
    Replaces the thread manager used by the task manager.
    Use this function if you want finer control 
    over how many threads the taskManager uses. By default the 
    TaskManager uses the global thread manager.
    
    For example, to limit the TaskManager to use one thread only,
    that doesn't count againt the global thread count, you can 
    do this:
    \code
        ThreadManager *myThreadMaanger = new ThreadManager();
        myTaskManager->setThreadLimit(1);
        taskManager->setThreadManager(myThredManager);
    \endcode

    Alternativelty, to make the threads used by the task manager count
    against the the global thread count, specify the global thread 
    manager as the parent of the new thread manager like this:
    \code
        ThreadManager *myThreadMaanger = new ThreadManager(ThreadManager::instance());
        ...
    \endcode

    Note that setting the thread manager is only supported while
    the task manager is idle. A good time to do this is right after
    the task manager is constructed.

    \sa ThreadManager
*/
void TaskManager::setThreadManager(ThreadManager *threadManager)
{
    Q_D(TaskManager);
    if (d->taskRunners.currentCount() != 0) {
        qWarning("TaskManager::setThreadManager: Setting a thread manager while the task manager is running is not supported.");
        return;
    }
    d->threadManager = threadManager;
}

/*!
    Returns the current thread manager for the task manager.
    \sa ThreadManager
*/
ThreadManager *TaskManager::threadManager() const
{
    Q_D(const TaskManager);
    return d->threadManager;
}

/*!
    Replaces the thread pool used by the task manager.
    This can be useful if you want the task manager to use
    a special pool of threads, for example a pool of threads
    where each thread has a database connection.

    \code
        ThreadPool *myThreadPool = new ThreadPool();
        taskManager->setThreadPool(myThreadPool);
    \endocode

    Note that setting the thread pool is only supported while
    the task manager is idle. A good time to do this is right after
    the task manager is constructed.

    \sa ThreadPool
*/
void TaskManager::setThreadPool(ThreadPool *threadPool)
{
    Q_D(TaskManager);
    if (d->taskRunners.currentCount() != 0) {
        qWarning("TaskManager::setThreadManager: Setting a thread pool while the task manager is running is not supported.");
        return;
    }

    d->threadPool = threadPool;
}

/*!
    Returns the a pointer queue object this task manager is using.
    
    Use this function if you want to do advanced operations
    on the queue beyond what the run() function supports.
    This includes removing and changing the priority of tasks.
        
    After you are done with the queue object, call start()
    to start task processing. This must be done even if
    the task manager is already processing tasks, in order
    to avoid race conditions where the task manager stops
    because the queue was temporarily empty while it was
    beeing modifed.
    
    The following example demonstrates how to hadd a task
    to the back of priority level 1 using this interface:
    \code
            MyTask *task = new MyTask();
            PriorityQueue<TaskBase *> *queue = taskManager->queue();
            QFuture<void> future = task->reportStart(); // Manually report that the task has started.
            queue->pushBack(task, 1);
            taskManager->start();
    \endcode
        
    \sa PriorityQueue, start()
*/
PriorityQueue<TaskBase *> *TaskManager::queue()
{
    Q_D(TaskManager);
    return &d->taskQueue;
}

/*!
    Makes the task manger start processing the tasks on the queue.
    
    Calling this function is only needed after modifying the queue directly
    using the queue() function. TaskManager::run() automatically starts
    task processing.
    
    \sa run(), queue()
*/
void TaskManager::start()
{
    createInitialThreadRunner();
}

/*! 
    \fn QFuture TaskManager::run(TaskType *task, int priority = 0);
    
    Adds \a task to the task manager queue at priority level \a priority.
    Returns a QFuture<T>, where T is the return type of the task (see below).

    Task types are craeted by creating a class that inherits from
    TaskManagerTask. TaskManagerTask taskes one template argument, 
    which specifies what type the task returns. This template argument
    can be any type that has a default constructor and a copy constructor,
    including void.

    \code
        class StringTask : public TaskManagerTask<QString>
        {
            QString run()
            {
                return QLatin1String("blogs.qtdeveloper.net");
            }
        };
    \endcode

    The task can then be added to the queue and run in a thread:
    \code
        QFuture<QString> future = TaskManager::instance()->run(new StringTask());
    \endcode
    \sa TaskManagerTask, QFuture
*/

/*  \overload
    \fn QFuture TaskManager::run(Functor functor, int priority = 0);
    
    Adds a function to the task manager queue. at priority level \a priority.
    Returns a QFuture<T>, where T is the return type of the function.

    \code
        QString stringFunction()
        {
            return QLatin1String("blogs.qtdeveloper.net");            
        }
        
        QFuture<QString> future = TaskManager::instance()->run(bind(stringFunction));
    \endcode

    bind() is used to turn the function into a function object
    that the task manager can use. See the bind documentation for further
    information.
    
    \sa bind, QFuture
*/

/*!
    Returns the current thread pool for the task manager.
    \sa ThreadPool
*/
ThreadPool *TaskManager::threadPool() const
{
    Q_D(const TaskManager);
    return d->threadPool;
}


void TaskManager::tryCreateThreadRunner()
{
    Q_D(TaskManager);
    if (d->taskQueue.isEmpty() == false && d->threadManager->tryReserveThread()) {
        createThreadRunner();
    }
}

void TaskManager::createInitialThreadRunner()
{
    Q_D(TaskManager);
    if (d->threadManager->reservedThreads() == 0) {
        d->threadManager->reserveThread();
        createThreadRunner();
    } else {
        tryCreateThreadRunner();
    }
}

void TaskManager::createThreadRunner()
{
    Q_D(TaskManager);
    d->taskRunners.acquire();
    d->threadPool->run(new TaskRunner(this));
}

void TaskManager::threadFinished()
{
    Q_D(TaskManager);
    d->threadManager->releaseThread();

    // Check if another thread just added a task to the queue, to avoid
    // race conditions where the last task runner will exit just after run()
    // has checked that there are task runners currently running.
    const bool lastThread = (d->taskRunners.release() == 0);
    if (lastThread && d->taskQueue.isEmpty() == false) {
        d->threadManager->reserveThread();
        createThreadRunner();
    }
}

void TaskManager::addTask(TaskBase *task, int priority)
{
    Q_D(TaskManager);
    // Implement warning here since we don't have access to d in run()
    if (d->isExiting)
        qWarning("TaskManager::run called after taskmanager object was deleted");

    d->taskQueue.pushBack(task, priority);
}

/*
    Called by task runners that are out of tasks
    on their internal queue.
*/
TaskManager::QueueResult TaskManager::requestTask()
{
    Q_D(TaskManager);
    return d->taskQueue.dequeue();
}

bool TaskManager::isTaskQueueEmpty()
{
    Q_D(TaskManager);
    return d->taskQueue.isEmpty();
}

/////////////////////////////////////////////////////////////////////////////////

bool runTask(TaskManager *taskManager)
{
    Result<TaskBase *> possibleTask = taskManager->requestTask();
    if (possibleTask.isValid() == false)
        return false;

    TaskBase *task = possibleTask.data();
    task->q_internalRun();
    taskManager->tryCreateThreadRunner();
    return true;
}

void TaskRunner::run()
{
    while (runTask(taskManager))
        ;
    
    taskManager->threadFinished();
    delete this;
}

} // namespace QtConcurrent
