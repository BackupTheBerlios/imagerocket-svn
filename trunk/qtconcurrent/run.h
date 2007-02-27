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
#ifndef QTCONCURRENT_RUN_H
#define QTCONCURRENT_RUN_H

#include <taskmanager.h>
#include <bind.h>

#ifdef qdoc
QFuture run(Function function);
#endif


namespace QtConcurrent {

template <typename TaskType>
QFuture<typename TaskType::ReturnType> run(TaskType *task, int priority = 0)
{
    return TaskManager::instance()->run(task, priority);
}

template <typename Functor>
QFuture<typename Functor::result_type> run(Functor functor, int priority = 0)
{
    return TaskManager::instance()->run(functor, priority);
}

template <typename T>
QFuture<T> run(T (fn)(), int priority = 0)
{
    return TaskManager::instance()->run(bind(fn), priority);
}

} // namespace QtConcurrent

#endif
