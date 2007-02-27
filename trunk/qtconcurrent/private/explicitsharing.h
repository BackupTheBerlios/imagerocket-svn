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
#ifndef QTCONCURRENT_EXPLISHITSHARING_H
#define QTCONCURRENT_EXPLISHITSHARING_H

namespace QtConcurrent {

template <class T> class QExplicitlySharedDataPointer
{
public:
    inline T *operator->() { return d; }
    inline const T *operator->() const { return d; }
    inline operator T *() { return d; }
    inline operator const T *() const { return d; }
    inline T *data() { return d; }
    inline const T *data() const { return d; }
    inline const T *constData() const { return d; }

    inline QExplicitlySharedDataPointer() : d(new T())
    { d->ref.ref(); }
    inline ~QExplicitlySharedDataPointer() { if (d && !d->ref.deref()) delete d; }
    explicit inline QExplicitlySharedDataPointer(T *data) :d(data)
    { if (d) d->ref.ref(); }
    inline QExplicitlySharedDataPointer(const QExplicitlySharedDataPointer &o) : d(o.d)
    { if (d) d->ref.ref(); }
    inline QExplicitlySharedDataPointer &operator=(const QExplicitlySharedDataPointer &o) {
        if (o.d != d) {
            T *x = o.d;
            if (x) x->ref.ref();
            x = qAtomicSetPtr(&d, x);
            if (x && !x->ref.deref())
                delete x;
        }
        return *this;
    }
    inline QExplicitlySharedDataPointer &operator=(T *o) {
        if (o != d) {
            T *x = o;
            if (x) x->ref.ref();
            x = qAtomicSetPtr(&d, x);
            if (x && !x->ref.deref())
                delete x;
        }
        return *this;
    }

    inline bool operator!() const { return !d; }

private:
    T *d;
};

} // namespace QtConcurrent

#endif
