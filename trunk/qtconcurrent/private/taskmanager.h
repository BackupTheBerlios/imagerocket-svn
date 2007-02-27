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
#ifndef QTCONCURRENT_TASKMANAGER_H
#define QTCONCURRENT_TASKMANAGER_H

#include "threadpool.h"
#include "stack.h"
#include "queue.h"
#include "future.h"
#include "futureaggregate.h"
#include "threadmanager.h"
#include "priorityqueue.h"

#include <QObject>
#include <QDebug>
#include <QSet>
#include <QReadWriteLock>
#include <QSemaphore>

namespace QtConcurrent {

class TaskManager;
class TaskRunner;

class TaskBase
{
public:
    virtual ~TaskBase() {};
    virtual void q_internalRun() = 0;
};

template <typename T>
class TypedTask : public TaskBase, public FutureEngineInterface<T>
{
public:
    typedef T ReturnType;
    virtual ReturnType run() = 0;
    bool wait(int);
    TaskManager *taskManager;
};

template< typename ReturnType>
class TaskManagerTask : public TypedTask<ReturnType>
{
public:
    void q_internalRun()
    {
        const ReturnType result = this->run();
        reportFinished(&result);
        this->deleteEngine();
    }
};

template<>
class TaskManagerTask<void> : public TypedTask<void>
{
public:
    void q_internalRun()
    {
        this->run();
        reportFinished();
        this->deleteEngine();
    }
};

class TaskRunner;
class TaskManagerPrivate;
class TaskManager : public QObject
{
    Q_OBJECT
public:
    typedef PriorityQueue<TaskBase *> QueueType;
    typedef QueueType::Result QueueResult;

    Q_DECLARE_PRIVATE(TaskManager)

    TaskManager(QObject *parent = 0);
    ~TaskManager();

    static TaskManager *instance();

    void setThreadManager(ThreadManager *threadManager);
    ThreadManager *threadManager() const;

    void setThreadPool(ThreadPool *threadPool);
    ThreadPool *threadPool() const;

    template <typename TaskType>
    QFuture<typename TaskType::ReturnType> run(TaskType *task, int priority = 0);

    template <typename Functor>
    QFuture<typename Functor::result_type> run(Functor functor, int priority = 0);

    template <typename TaskType, typename Sequence>
    QFuture<typename TaskType::ReturnType> mapEach(const Sequence &sequence);
    
    template <typename Functor, typename Sequence>
    QFuture<typename Functor::result_type> mapEach(const Sequence &sequence, Functor functor);

    PriorityQueue<TaskBase *> *queue();
    void start();

    bool isTaskQueueEmpty(); // not really public

    friend class TaskRunner;
    friend bool runTask(TaskManager *taskManager);
private:
    void tryCreateThreadRunner();
    void createInitialThreadRunner();
    void createThreadRunner();
    void threadFinished();
    void addTask(TaskBase *task, int priority);
    QueueResult requestTask();


    TaskManagerPrivate *d;
};

template <typename TaskType>
QFuture<typename TaskType::ReturnType> TaskManager::run(TaskType *task, int priority)
{
    task->taskManager = this;
    QFuture<typename TaskType::ReturnType> future = task->reportStart();
    addTask(task, priority);
    createInitialThreadRunner();
    return future;
}

template <typename Functor>
class FunctorTask : public TaskManagerTask<typename Functor::result_type>
{
public:
    FunctorTask(Functor &functor) : functor(functor) { }
    typename Functor::result_type run()
    { 
        if (this->isCanceled())
            return typename Functor::result_type();
        return functor();
    }
private:
    Functor functor;
};

template <typename Functor>
QFuture<typename Functor::result_type> TaskManager::run(Functor functor, int priority)
{
    return run(new FunctorTask<Functor>(functor), priority);
}

template <typename TaskType, typename Sequence>
QFuture<typename TaskType::ReturnType> TaskManager::mapEach(const Sequence &sequence)
{
    QAggregatedFuture<typename TaskType::ReturnType> future;
    typename Sequence::const_iterator it = sequence.begin();
    const typename Sequence::const_iterator end = sequence.end();
    while (it != end)
        future.addFuture(run(new TaskType(*it++)));
    return future;
}

template <typename Functor, typename Sequence>
QFuture<typename Functor::result_type> TaskManager::mapEach(const Sequence &sequence, Functor functor)
{
    QAggregatedFuture<typename Functor::result_type> future;
    typename Sequence::const_iterator it = sequence.begin();
    const typename Sequence::const_iterator end = sequence.end();

    while (it != end)
        future.addFuture(run(bind(functor, *it++)));
    return future;
}


template <typename T>
bool TypedTask<T>::wait(int)
{
    runTask(taskManager);
    return (taskManager->isTaskQueueEmpty() == false);
}

} // namespace QtConcurrent

#endif
