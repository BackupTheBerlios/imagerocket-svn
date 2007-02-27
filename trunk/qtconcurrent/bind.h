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
#ifndef QTCONCURRENT_BIND_H
#define QTCONCURRENT_BIND_H

#include "3rdparty/boost/bind.hpp"
#include "map.h"

namespace QtConcurrent
{
    using namespace boost;

    // Make boost::bind work with Q*SequenteItems

    template <typename T>
    const T *get_pointer(QSequenceItem<T> &p)
    {
        return &p.value();
    }

    template <typename T>
    const T *get_pointer(const QSequenceItem<T> &p)
    {
        return &p.value();
    }

    template <typename T>
    T *get_pointer(QMutableSequenceItem<T> &p)
    {
        return &p.value();
    }

}

// Additions to boost::bind that allows omitting place holders
// placed leftmost in bind calls. For example, bind(fn, _1, foo)
// is equivalent to bind(fn, foo).
namespace boost
{

#define BOOST_BIND_CC
#define BOOST_BIND_ST

// B1 is the bound argument
// _1 (placeholder) = argument to bind
template<class R, class B1>
    _bi::bind_t<R, BOOST_BIND_ST R (BOOST_BIND_CC *) (B1), typename _bi::list_av_1<boost::arg<1> >::type>
    BOOST_BIND(BOOST_BIND_ST R (BOOST_BIND_CC *f) (B1))
{
    typedef BOOST_BIND_ST R (BOOST_BIND_CC *F) (B1);
    typedef typename _bi::list_av_1<boost::arg<1> >::type list_type;
    return _bi::bind_t<R, F, list_type> (f, list_type(_1));
}

template<class R, class B1, class B2, class A1>
    _bi::bind_t<R, BOOST_BIND_ST R (BOOST_BIND_CC *) (B1, B2), typename _bi::list_av_2<A1, boost::arg<1> >::type>
    BOOST_BIND(BOOST_BIND_ST R (BOOST_BIND_CC *f) (B1, B2), A1 a1)
{
    typedef BOOST_BIND_ST R (BOOST_BIND_CC *F) (B1, B2);
    typedef typename _bi::list_av_2<A1, boost::arg<1> >::type list_type;
    return _bi::bind_t<R, F, list_type> (f, list_type(a1, _1));
}

// B1 is the first bound argument
// B2 is the second bound argument
// _1 (placeholder) = argument to bind
// _2 (placeholder) = argument to bind
template<class R, class B1, class B2>
    _bi::bind_t<R, BOOST_BIND_ST R (BOOST_BIND_CC *) (B1, B2), typename _bi::list_av_2<boost::arg<1>, boost::arg<2> >::type>
    BOOST_BIND(BOOST_BIND_ST R (BOOST_BIND_CC *f) (B1, B2))
{
    typedef BOOST_BIND_ST R (BOOST_BIND_CC *F) (B1, B2);
    typedef typename _bi::list_av_2<boost::arg<1>, boost::arg<2> >::type list_type;
    return _bi::bind_t<R, F, list_type> (f, list_type(_1, _2));
}

#undef BOOST_BIND_CC
#undef BOOST_BIND_ST

#define BOOST_BIND_MF_NAME(X) X
#define BOOST_BIND_MF_CC

// bind(&Foo::bar)(&foo)
template<class R, class T>
    _bi::bind_t<R, _mfi::BOOST_BIND_MF_NAME(mf0)<R, T>, typename _bi::list_av_1<boost::arg<1> >::type>
    BOOST_BIND(R (BOOST_BIND_MF_CC T::*f)())
{
    typedef _mfi::BOOST_BIND_MF_NAME(mf0)<R, T> F;
    typedef typename _bi::list_av_1<boost::arg<1> >::type list_type;
    return _bi::bind_t<R, F, list_type>(F(f), list_type(_1));
}

// bind(&Foo::bar)(&foo) (const version)
template<class R, class T>
    _bi::bind_t<R, _mfi::BOOST_BIND_MF_NAME(cmf0)<R, T>, typename _bi::list_av_1<boost::arg<1> >::type>
    BOOST_BIND(R (BOOST_BIND_MF_CC T::*f)() const)
{
    typedef _mfi::BOOST_BIND_MF_NAME(cmf0)<R, T> F;
    typedef typename _bi::list_av_1<boost::arg<1> >::type list_type;
    return _bi::bind_t<R, F, list_type>(F(f), list_type(_1));
}

// bind(&Foo::bar, 1)(&foo)
template<class R, class T,
    class B1, class A1>
    _bi::bind_t<R, _mfi::BOOST_BIND_MF_NAME(mf1)<R, T, B1>, typename _bi::list_av_2<boost::arg<1>, A1 >::type>
    BOOST_BIND(R (BOOST_BIND_MF_CC T::*f) (B1), A1 a1)
{
    typedef _mfi::BOOST_BIND_MF_NAME(mf1)<R, T, B1> F;
    typedef typename _bi::list_av_2<boost::arg<1>, A1 >::type list_type;
    return _bi::bind_t<R, F, list_type>(F(f), list_type(_1, a1));
}

// bind(&Foo::bar, 1)(&foo) (const version)
template<class R, class T,
    class B1, class A1>
    _bi::bind_t<R, _mfi::BOOST_BIND_MF_NAME(cmf1)<R, T, B1>, typename _bi::list_av_2<boost::arg<1>, A1 >::type>
    BOOST_BIND(R (BOOST_BIND_MF_CC T::*f) (B1) const, A1 a1)
{
    typedef _mfi::BOOST_BIND_MF_NAME(cmf1)<R, T, B1> F;
    typedef typename _bi::list_av_2<boost::arg<1>, A1 >::type list_type;
    return _bi::bind_t<R, F, list_type>(F(f), list_type(_1, a1));
}

// bind(&Foo::bar, 1, 2)(&foo)
template<class R, class T,
    class B1, class B2,
    class A1, class A2>
    _bi::bind_t<R, _mfi::BOOST_BIND_MF_NAME(mf2)<R, T, B1, B2>, typename _bi::list_av_3<boost::arg<1>, A1, A2>::type>
    BOOST_BIND(R (BOOST_BIND_MF_CC T::*f) (B1, B2), A1 a1, A2 a2)
{
    typedef _mfi::BOOST_BIND_MF_NAME(mf2)<R, T, B1, B2> F;
    typedef typename _bi::list_av_3<boost::arg<1>, A1, A2>::type list_type;
    return _bi::bind_t<R, F, list_type>(F(f), list_type(_1, a1, a2));
}

// bind(&Foo::bar, 1, 2)(&foo) (const version)
template<class R, class T,
    class B1, class B2,
    class A1, class A2>
    _bi::bind_t<R, _mfi::BOOST_BIND_MF_NAME(cmf2)<R, T, B1, B2>, typename _bi::list_av_3<boost::arg<1>, A1, A2>::type>
    BOOST_BIND(R (BOOST_BIND_MF_CC T::*f) (B1, B2) const, A1 a1, A2 a2)
{
    typedef _mfi::BOOST_BIND_MF_NAME(cmf2)<R, T, B1, B2> F;
    typedef typename _bi::list_av_3<boost::arg<1>, A1, A2>::type list_type;
    return _bi::bind_t<R, F, list_type>(F(f), list_type(_1, a1, a2));
}

#undef BOOST_BIND_MF_NAME
#undef BOOST_BIND_MF_CC

} // namepespace boost

#endif
