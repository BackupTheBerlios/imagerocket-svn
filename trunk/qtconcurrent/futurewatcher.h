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
#ifndef QTCONFURREFUTUREOBJECT_H
#define QTCONFURREFUTUREOBJECT_H

#include "future.h"

#ifdef qdoc
class QFutureWatcher : public QObject
{ 
signals:
    void started();
    void finished();
    void canceled();
    void resultReadyAt(int resultIndex);
    void resultReady(const QVariant &result);
    void progressRangeChanged(int minimum, int maximum);
    void progressValueChanged(int progressValue);
};
#endif

namespace QtConcurrent {

class QFutureWatcher : public QObject, public EngineOutputInterface
{
    Q_OBJECT
public:
    QFutureWatcher(QObject *parent = 0);
    ~QFutureWatcher();
Q_SIGNALS:
    void started();
    void finished();
    void canceled();

    void resultReadyAt(int resultIndex);
    void resultReady(const QVariant &result);
    void progressRangeChanged(int minimum, int maximum);
    void progressValueChanged(int progressValue);
public Q_SLOTS:
    void cancel();
public:
    void setFuture(const FutureBase &future);
    FutureBase future();
    int progressValue() const;
    int progressMinimum() const;
    int progressMaximum() const;
protected:
    void started(void *source);
    void finished(void *source);
    void canceled(void *source);
    void resultReady(int index, void *source);
    void progressRange(int minimum, int maximum, void *source);
    void progress(int progress, void *source);

    FutureBase m_future;
};

} //namespace QtConcurrent

#endif
