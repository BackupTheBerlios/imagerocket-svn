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

#include "futurewatcher.h"

/*! \class QFutureWatcher
    \reentrant
    \brief The QFutureWatcher allows monitoring a QFuture using signals and slots.
    
    There are several status-reporting signals such as finished(), progress() and
    resultReady(), and there is one slot: cancel(). Note that not all futures can
    be canceled. A future returned by map() can, while a future returned by running
    a function with run() can't.
    
    Example: Starting a computation and getting a slot callback when it's finished:
\code 
    // Instantiate the objects and connect to the finished signal.
    MyClass myObject;
    QFutureWatcher<int> watcher;
    connect(&watcher, SIGNAL(finsihed), &myObject, SLOT(handleFinished));

    // Start the computation.
    int foo(); // defined somwhere else.
    QFuture<int> future = run(foo);
    watcher.setFuture(future);
\endcode
    \sa QFuture
*/

/*!
    \fn void QFutureWatcher::started()
    This signal is emittet when the watched future starts.
*/

/*!
    \fn void QFutureWatcher::finished()
    This signal is emittet when the watched future finishes.
*/

/*!
    \fn void QFutureWatcher::canceled()
    This signal is emittet if the watched future is canceled.
*/

/*!
    \fn void QFutureWatcher::progressRangeChanged(int minimum, int maximum)
    The progress range for the watched future has changed 
    to \a minimum and \a maximum
*/

/*!
    \fn void QFutureWatcher::progressValueChanged(int progressValue)
    This signal is emittet when the watched future reports progress, 
    \a progressValue gives the current progress. In order to avoid overloading
    the GUI event loop, QFutureWathcer limits the progress signal emission rate.
    This means that listeners connected to this slot might not get all progress 
    reports the future makes. The last progress update (where progressValue  ==
    maximum) will always be delivered.
*/

/*!
    \fn void QFutureWatcher::resultReadyAt(int index)
    This signal is emittet when the watched future reports a ready result at \a index.
    If the future reports mutltiple results, the index will indicate witch
    one it is. Results can be reported out-of-order. To get the result, 
    call future.result(index)
*/


/*!
    \fn void QFutureWatcher::resultReady(const QVariant &result)
    This signal is emittet when the watched future reports a ready \a result.
    
*/

namespace QtConcurrent {

QFutureWatcher::QFutureWatcher(QObject *parent)
:QObject(parent) {}

QFutureWatcher::~QFutureWatcher()
{
    m_future.resultHolder->disconnectOutputInterface(this);
}

/*!
    Cancels the watched future.
    \sa QFture::cancel
*/
void QFutureWatcher::cancel()
{
    m_future.cancel();
}

void QFutureWatcher::setFuture(const FutureBase &future)
{
    if (m_future.resultHolder == future.resultHolder)
        return;

    m_future.resultHolder->disconnectOutputInterface(this);
    m_future = future;
    m_future.resultHolder->connectOutputInterface(this);
}

FutureBase QFutureWatcher::future()
{
    return m_future;
}

/*!
    \internal
*/
void QFutureWatcher::started(void *source)
{
    Q_UNUSED(source);
    emit started();
}

/*!
    \internal
*/
void QFutureWatcher::finished(void *source)
{
    Q_UNUSED(source);
    emit finished();
}

/*!
    \internal
*/
void QFutureWatcher::canceled(void *source)
{
    Q_UNUSED(source);
    emit canceled();
}

/*!
    \internal
*/
void QFutureWatcher::resultReady(int index, void *source)
{
    Q_UNUSED(source);
    emit resultReadyAt(index);
    emit resultReady(m_future.variantResultAt(index));
}

/*!
    \internal
*/
void QFutureWatcher::progressRange(int minimum, int maximum, void *source)
{
    Q_UNUSED(source);
    emit progressRangeChanged(minimum, maximum);
}

/*!
    \internal
*/
void QFutureWatcher::progress(int progressValue, void *source)
{
    Q_UNUSED(source);
    emit progressValueChanged(progressValue);
}

} // namespace QtConcurrent
