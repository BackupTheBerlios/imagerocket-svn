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
#ifndef QTCONCURRENT_FUTURESHARED_H
#define QTCONCURRENT_FUTURESHARED_H

#include <QDebug>

namespace QtConcurrent {

enum State { Null = 0, Running = 1, Started = 2, Finished = 4, Canceled = 8, Paused = 16 };

/*
    The EngineOutput interface is implemented by classes
    that wants to handle or relay result and state update 
    calls from the engine.
*/
class EngineOutputInterface
{
public:
    virtual ~EngineOutputInterface() {}
    virtual void started(void *source) { Q_UNUSED(source); }
    virtual void finished(void *source) { Q_UNUSED(source); }
    virtual void canceled(void *source) { Q_UNUSED(source); }
    virtual void paused(void *source) { Q_UNUSED(source); }
    virtual void resultReady(int index, void *source) { Q_UNUSED(index); Q_UNUSED(source);}
    virtual void progressRange(int begin, int end, void *source) { Q_UNUSED(begin); Q_UNUSED(end); Q_UNUSED(source);}
    virtual void progress(int progress, void *source) { Q_UNUSED(progress); Q_UNUSED(source); }
};

/*
    The EngineInput interface is implemented by the engine 
    itself, as well as classes that wants to relay calls
    to the engine.
*/
class EngineInputInterface
{
public:
    virtual ~EngineInputInterface() {}
    virtual bool wait(int resultIndex) { Q_UNUSED(resultIndex); return false; }
    virtual void cancel(int resultIndex) { Q_UNUSED(resultIndex); }
};

}

#endif
