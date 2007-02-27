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
/*!
    \headerfile <run.h>
*/

/*!
\fn QFuture run(Function function)
\relates <run.h>

Runs \a function in a new thread. If the maximimum number of threads
is already in use, the function call is put on a queue.

Example: Starting a computation and then synchronizing on the result:
\code 
int foo()
{
    return 0;
}

QFuture<int> future = run(foo);

// other code here

qDebug() << "the result is" << future.result();
\endcode

If you want to run a function that takes arguments, use
bind() to provide a value for those arguments:

\code 
int foo(int argument)
{
    return argument;
}

QFuture<int> future = run(bind(foo, 9));

// other code here

qDebug() << "the result is" << future.result();
\endcode




*/