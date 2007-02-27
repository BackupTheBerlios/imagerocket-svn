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
#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include <QtCore/QtGlobal>

namespace QtConcurrent {

    class ThreadManagerPrivate;

    class ThreadManager
    {
    public:
        ThreadManager(ThreadManager *parent = 0);
        ~ThreadManager();

        static ThreadManager *instance();

        ThreadManager *parent() const;

        int threadLimit() const;
        void setThreadLimit(int threadLimit);

        bool tryReserveThread();
        void reserveThread();
        int releaseThread();
        int reservedThreads() const;

    private:
        ThreadManagerPrivate *d;
    };

} // namespace QtConcurrent

#endif // THREADMANAGER_H
