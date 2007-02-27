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
#include "threadmanager.h"

#include <QAtomic>
#include <QThread>


# if QT_VERSION < 0x040300

#if defined(Q_OS_WIN32)
# include <Windows.h>
#else
# include <unistd.h>
# include <netinet/in.h>
# include <sys/utsname.h>
# include <sys/socket.h>
#  if defined(Q_OS_MACX)
#   include <CoreServices/CoreServices.h>
#  elif defined(Q_OS_HPUX)
#   include <sys/pstat.h>
#  elif defined(Q_OS_FREEBSD) || defined(Q_OS_OPENBSD) || defined(Q_OS_MAC)
#   include <sys/sysctl.h>
#  endif
#endif

#endif

namespace QtConcurrent {

# if QT_VERSION < 0x040300
int idealThreadCount()
{
    static int pc = 0;
    if ( pc == 0 ) {
#if defined(Q_OS_WIN32)
	// Windows
	SYSTEM_INFO sysinfo;
	GetSystemInfo( &sysinfo );
	pc = (int)sysinfo.dwNumberOfProcessors;
#elif defined(Q_OS_MACX)
	// Mac OS X
	pc = MPProcessorsScheduled();
#elif defined(Q_OS_HPUX)
	// HP-UX
	struct pst_dynamic psd;
	if ( pstat_getdynamic( &psd, sizeof(psd), 1, 0 ) == -1 ) {
	    perror( "pstat_getdynamic" );
	    pc = 1;
	} else {
	    pc = (int)psd.psd_proc_cnt;
	}
#elif defined(Q_OS_FREEBSD) || defined(Q_OS_OPENBSD) || defined(Q_OS_NETBSD) || defined(Q_OS_BSDI) || defined(Q_OS_MAC)
	// FreeBSD, OpenBSD, NetBSD, BSD/OS
	size_t len = sizeof( pc );
	int mib[2];
	mib[0] = CTL_HW;
	mib[1] = HW_NCPU;
	if ( sysctl( mib, 2, &pc, &len, NULL, 0 ) != 0 ) {
	    perror( "sysctl" );
	    pc = 1;
	}
#elif defined(Q_OS_IRIX)
	// IRIX
	pc = (int)sysconf(_SC_NPROC_ONLN);
#else
	// Linux, Solaris, AIX, Tru64
	pc = (int)sysconf(_SC_NPROCESSORS_ONLN);
#endif
	if ( pc < 1 )
	    pc = 1;
    }
    return pc;
}

#endif

    class ThreadManagerPrivate
    {
    public:
        ThreadManager *parent;
        QAtomic threadCount;
        int threadLimit;

        ThreadManagerPrivate(ThreadManager *parent)
#if QT_VERSION >= 0x040300
            : parent(parent), threadCount(0), threadLimit(QThread::idealThreadCount())
#else
            : parent(parent), threadCount(0), threadLimit(idealThreadCount())
#endif
        { }
    };

} // namespace QtConcurrent

Q_GLOBAL_STATIC(QtConcurrent::ThreadManager, globalThreadManager)

/*!
    \class QtConcurrent::ThreadManager
    \brief provides an interface to control the number of threads used by QtConcurrent.
    \threadsafe

    QtConcurrent uses the global ThreadManager to manage the number of
    threads used in the global ThreadPool. You can use this interface
    for your own threads, as well. The intent is to prevent your
    application from creating too many threads, which can cause poor
    performance. The global ThreadManager is accessible using the
    instance() function.
  
    Each ThreadManager instance can have a parent(), and when a
    ThreadManager reserves and releases threads, the threads are also
    reserved and released in the parent(). Note that deleting a
    ThreadManager does \e not delete its children.

    Call threadLimit() to query the maximum number of threads to be
    used. If needed, you can change the limit with
    setThreadLimit(). The default threadLimit() is
    QThread::idealThreadCount();

    The tryReserveThread() and reserveThread() functions reserve
    threads. Use releaseThread() when your are done with the thread,
    so that it may be reused.  The reservedThreads() function returns
    the number of reserved threads.

    For example, if you are starting a background processing thread
    and want QtConcurrent to take that thread into account when
    deciding how many threads to create, call 
    ThreadManager::instance()->reserveThread() when the background 
    processing begins and then ThreadManager::instance()->freeThread()
    when it ends.

    \sa ThreadPool
*/

QtConcurrent::ThreadManager::ThreadManager(ThreadManager *parent)
    : d(new ThreadManagerPrivate(parent))
{ }

QtConcurrent::ThreadManager::~ThreadManager()
{
    delete d;
}

/*!
    Returns the global ThreadManager.
*/
QtConcurrent::ThreadManager *QtConcurrent::ThreadManager::instance()
{
    return ::globalThreadManager();
}

/*!
    Returns the parent for this ThreadManager.
*/
QtConcurrent::ThreadManager *QtConcurrent::ThreadManager::parent() const
{
    return d->parent;
}

/*!
    Returns the thread limit, which represents the maximum number of
    threads used by QtConcurrent.

    This function returns the limit previously set by
    setThreadLimit(). If setThreadLimit() has not been called, this
    function returns the default thread limit, which is the same as
    QThread::idealThreadCount().

    \sa QThread::idealThreadCount()
*/
int QtConcurrent::ThreadManager::threadLimit() const
{
    return d->threadLimit;
}

/*!
    Sets the thread limit to \a threadLimit.

    Setting the thread limit effectively controls the maximum number
    of threads used by QtConcurrent. Note that QtConcurrent will
    always use at least 1 thread, even if \a threadLimit is zero or
    negative.
 */
void QtConcurrent::ThreadManager::setThreadLimit(int threadLimit)
{
    d->threadLimit = threadLimit;
}

/*!
    Try to reserves one thread. The attempt succeeds if an attempt to
    reserve a thread in the parent() succeeds and reservedThreads() is
    less than the threadLimit(). This function returns true if a
    thread could be reserved; false otherwise.

    Once you are done with the thread, call releaseThread() to allow
    it to be reused.

    \sa releaseThread(), reserveThread()
 */
bool QtConcurrent::ThreadManager::tryReserveThread()
{
    int localThreadCount = d->threadCount;
    forever {
        if (localThreadCount >= d->threadLimit) {
            return false;
        }
        if (d->threadCount.testAndSet(localThreadCount, localThreadCount + 1))
            break;
        localThreadCount = d->threadCount;
    }

    if (d->parent && !d->parent->tryReserveThread()) {
        // failed to get a thread in the parent, so release our thread
        (void) d->threadCount.deref();
        return false;
    }

    return true;
}

/*!
    Reserves one thread, disregarding reservedThreads() and
    threadLimit(). The thread is also reserved in the parent().

    Once you are done with the thread, call releaseThread() to allow
    it to be reused.

    Note that unlike tryReserveThread(), this function will always
    increase the number of reserved threads. This means it is possible
    to reserved more than threadLimit() threads by using this
    function.

    \sa tryReserveThread(), releaseThread()
 */
void QtConcurrent::ThreadManager::reserveThread()
{
    (void) d->threadCount.ref();

    if (d->parent)
        d->parent->reserveThread();
}

/*!
    Releases one reserved thread. The thread is also released in the parent().
    
    This function returns the number of reserved threads after the release,
    i.e. if the last thread has been released, this function returns 0.

    If no threads have been reserved, this function does nothing. In
    other words, this function will decreased reservedThreads() only
    if reserveThread() or tryReserveThread() has been called.

    \sa tryReserveThread(), reserveThread()
*/
int QtConcurrent::ThreadManager::releaseThread()
{
    // decrement threadCount, making sure it never goes below zero
    int localThreadCount = d->threadCount;
    forever {
        if (localThreadCount <= 0)
            return -1;
        if (d->threadCount.testAndSet(localThreadCount, localThreadCount - 1))
            break;
        localThreadCount = d->threadCount;
    }

    if (d->parent)
        d->parent->releaseThread();
    return localThreadCount - 1;
}

/*!
    Returns the number of threads reserved by tryReserveThread() and
    reserveThread().

    Note that is is possible for this function to return a value that
    is greater than threadLimit(). See reserveThread() for more
    details.

    \sa tryReserveThread(), reserveThread()
*/
int QtConcurrent::ThreadManager::reservedThreads() const
{
    return d->threadCount;
}
