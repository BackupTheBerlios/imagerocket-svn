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
#ifndef QTCONCURRENT_THREADPOOL_H
#define QTCONCURRENT_THREADPOOL_H

#include <QThread>

namespace QtConcurrent {

class ThreadPoolTask
{
public:
    virtual void run() = 0;
    virtual ~ThreadPoolTask() {};
};

class ThreadPoolPrivate;
class ThreadPool
{
public:
    ThreadPool();
    ~ThreadPool();
    static ThreadPool *instance();
    void run(ThreadPoolTask *task, QThread::Priority priority = QThread::NormalPriority);

    int expiryTimeout() const;
    void setExpiryTimeout(int expiryTimeout);

private:
    ThreadPoolPrivate *d;
};

} // namespace QtConcurrent

#endif
