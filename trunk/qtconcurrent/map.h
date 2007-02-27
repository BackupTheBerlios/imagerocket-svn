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
#ifndef QTCONCURRET_MAP_H
#define QTCONCURRET_MAP_H

#include <for.h>
#include <QStringList>
#include "stack.h"
#include "simplefunctionwrappers.h"

#ifdef qdoc

QFuture map(const Sequence &sequence, MapFunction function);
QFuture mapped(const Sequence &sequence, MapFunction function);
QFuture mappedReduced(const Sequence &sequence, MapFunction function, ReduceFunction function);
#endif

namespace QtConcurrent {

/*
    QSequenceItem and QMutableSequenceItem contain progress
    information (which index in the sequence) in addition to a
    reference to the item being mapped.
*/
template <typename T>
class QSequenceItem
{
public:
    QSequenceItem(const T &value, int index)
        : v(value), i(index)
    { }

    const T &value() const
    {
        return v;
    }

    int index() const
    {
        return i;
    }

    operator const T &() const
    {
        return v;
    }

private:
    const T &v;
    int i;
};

// See above
template <typename T>
class QMutableSequenceItem
{
public:
    inline QMutableSequenceItem(T &value, int index)
        : v(value), i(index)
    { }

    T &value()
    {
        return v;
    }

    int index() const
    {
        return i;
    }

    operator T &()
    {
        return v;
    }

private:
    T &v;
    int i;
};




// Implementation of map
template <typename Sequence, typename MapFunctor>
class MapFor : public VoidFor
{
    Sequence &sequence;
    MapFunctor map;

public:
    MapFor(Sequence &sequence, MapFunctor map)
        : sequence(sequence), map(map)
    { }

    void runIterations(int begin, int end)
    {
        while (begin != end) {
            QMutableSequenceItem<typename Sequence::value_type> item(sequence[begin], begin);
            map(item);
            ++begin;
        }
    }
};

template <typename Sequence, typename MapFunctor>
QFuture<void> map(Sequence &sequence, MapFunctor map)
{
    const MapFor<Sequence, MapFunctor> mapFor(sequence, map);
    return QtConcurrent::runFor(0, sequence.size(), mapFor);
}

template <typename Sequence, typename T, typename U>
QFuture<void> map(Sequence &sequence, T (map)(U))
{
    return QtConcurrent::map(sequence, FunctionWrapper1<T, U>(map));
}

template <typename Sequence, typename T, typename C>
QFuture<void> map(Sequence &sequence, T (C::*map)())
{
    return QtConcurrent::map(sequence, MemberFunctionWrapper<T, C>(map));
}




// MapResults holds a block of results from a map functor. The offset
// indicates the origin of the block.
template <typename T>
class MapResults
{
public:
    int offset;
    QList<T> results;
};

// Simple, out-of-order reduction
template <typename ReduceFunctor, typename ReduceResultType, typename T>
class SimpleReducer
{
    QAtomic reduceThread;
    Stack<MapResults<T> > resultsStack;

    void reduceResult(ReduceFunctor &reduce, ReduceResultType &r, const MapResults<T> &result)
    {
        for (int i = 0; i < result.results.size(); ++i) {
            QSequenceItem<T> item(result.results[i], result.offset + i);
            reduce(r, item);
        }
    }

public:
    SimpleReducer()
        : reduceThread(0)
    { }

    void runReduce(ReduceFunctor &reduce, ReduceResultType &r, const MapResults<T> &result)
    {
        if (reduceThread.testAndSet(0, 1)) {
            reduceResult(reduce, r, result);
            finish(reduce, r);
            reduceThread.deref();
        } else {
            resultsStack.push(result);
        }
    }

    void finish(ReduceFunctor &reduce, ReduceResultType &r)
    {
        forever {
            typename Stack<MapResults<T> >::Result d = resultsStack.pop();
            if (!d.isValid())
                break;
            reduceResult(reduce, r, d.data());
        }
    }
};

// Ordered reduction
template <typename ReduceFunctor, typename ReduceResultType, typename T>
class OrderedReducer
{
    QMutex *mutex;
    int progress;
    QMap<int, MapResults<T> > resultsMap;

    void reduceResult(ReduceFunctor &reduce, ReduceResultType &r, const MapResults<T> &result)
    {
        for (int i = 0; i < result.results.size(); ++i) {
            QSequenceItem<T> item(result.results[i], result.offset + i);
            reduce(r, item);
        }
    }

public:
    OrderedReducer()
        : mutex(0), progress(0)
    { }

    OrderedReducer(const OrderedReducer &other)
        : mutex(new QMutex), progress(other.progress), resultsMap(other.resultsMap)
    { }

    ~OrderedReducer()
    {
        delete mutex;
    }

    void runReduce(ReduceFunctor &reduce, ReduceResultType &r, const MapResults<T> &result)
    {
        QMutexLocker locker(mutex);
        if (progress == result.offset) {
            // reduce this result
            locker.unlock();
            reduceResult(reduce, r, result);
            locker.relock();

            progress += result.results.size();

            // reduce as many other results as possible
            typename QMap<int, MapResults<T> >::iterator it = resultsMap.begin();
            while (it != resultsMap.end()) {
                if (it.value().offset != progress)
                    break;

                locker.unlock();
                reduceResult(reduce, r, it.value());
                locker.relock();

                progress += result.results.size();
                it = resultsMap.erase(it);
            }
        } else {
            // reduce later
            resultsMap.insert(result.offset, result);
        }
    }

    // final reduction
    void finish(ReduceFunctor &reduce, ReduceResultType &r)
    {
        foreach (const MapResults<T> &result, resultsMap) {
            reduceResult(reduce, r, result);
        }
    }
};

enum ReduceOptions { SimpleReduce, OrderedReduce };

// Implementation of map-reduce
template <typename ReducedResultType,
          typename Sequence,
          typename MapFunctor,
          typename ReduceFunctor,
          typename Reducer = SimpleReducer<ReduceFunctor,
                                           ReducedResultType,
                                           typename MapFunctor::result_type> >
class MappedReducedFor
{
    ReducedResultType reducedResult;
    const Sequence sequence;
    MapFunctor map;
    ReduceFunctor reduce;
    Reducer reducer;

public:
    MappedReducedFor(const Sequence &sequence, MapFunctor map, ReduceFunctor reduce)
        : reducedResult(), sequence(sequence), map(map), reduce(reduce)
    { }

    MappedReducedFor(ReducedResultType initialValue,
                     const Sequence &sequence,
                     MapFunctor map,
                     ReduceFunctor reduce)
        : reducedResult(initialValue), sequence(sequence), map(map), reduce(reduce)
    { }

    void runIterations(int begin, int end)
    {
        MapResults<typename MapFunctor::result_type> mapResults;
        mapResults.offset = begin;

        while (begin != end) {
            QSequenceItem<typename Sequence::value_type> item(sequence[begin], begin);
            mapResults.results.append(map(item));
            ++begin;
        }

        reducer.runReduce(reduce, reducedResult, mapResults);
    }

    void finish()
    {
        reducer.finish(reduce, reducedResult);
    }

    typedef ReducedResultType ResultType;
    ReducedResultType *result()
    {
        return &reducedResult;
    }
};

// mappedReduced()
template <typename ResultType, typename Sequence, typename MapFunctor, typename ReduceFunctor>
QFuture<ResultType> mappedReduced(const Sequence &sequence,
                                  MapFunctor map,
                                  ReduceFunctor reduce,
                                  ReduceOptions options = SimpleReduce)
{
    if (options == OrderedReduce) {
        typedef OrderedReducer<ReduceFunctor, ResultType, typename MapFunctor::result_type> Reducer;
        const MappedReducedFor<ResultType, Sequence, MapFunctor, ReduceFunctor, Reducer>
            mapFor(sequence, map, reduce);
        return QtConcurrent::runFor(0, sequence.size(), mapFor);
    }
    const MappedReducedFor<ResultType, Sequence, MapFunctor, ReduceFunctor>
        mapFor(sequence, map, reduce);
    return QtConcurrent::runFor(0, sequence.size(), mapFor);
}

template <typename Sequence, typename MapFunctor, typename T, typename U, typename V>
QFuture<U> mappedReduced(const Sequence &sequence,
                         MapFunctor map,
                         T (reduce)(U &, V),
                         ReduceOptions options = SimpleReduce)
{
    return mappedReduced<U>(sequence,
                            map,
                            FunctionWrapper2<T, U &, V>(reduce),
                            options);
}

template <typename Sequence, typename MapFunctor, typename T, typename C, typename U>
QFuture<C> mappedReduced(const Sequence &sequence,
                         MapFunctor map,
                         T (C::*reduce)(U),
                         ReduceOptions options = SimpleReduce)
{
    return mappedReduced<C>(sequence,
                            map,
                            MemberFunctionWrapper1<T, C, U>(reduce),
                            options);
}

template <typename ResultType, typename Sequence, typename T, typename U, typename ReduceFunctor>
QFuture<ResultType> mappedReduced(const Sequence &sequence,
                                  T (map)(U),
                                  ReduceFunctor reduce,
                                  ReduceOptions options = SimpleReduce)
{
    return mappedReduced<ResultType>(sequence,
                                     FunctionWrapper1<T, U>(map),
                                     reduce,
                                     options);
}

template <typename ResultType, typename Sequence, typename T, typename C, typename ReduceFunctor>
QFuture<ResultType> mappedReduced(const Sequence &sequence,
                                  T (C::*map)() const,
                                  ReduceFunctor reduce,
                                  ReduceOptions options = SimpleReduce)
{
    return mappedReduced<ResultType>(sequence,
                                     ConstMemberFunctionWrapper<T, C>(map),
                                     reduce,
                                     options);
}

template <typename Sequence, typename T, typename U, typename V, typename W, typename X>
QFuture<W> mappedReduced(const Sequence &sequence,
                         T (map)(U),
                         V (reduce)(W &, X),
                         ReduceOptions options = SimpleReduce)
{
    return mappedReduced<W>(sequence,
                            FunctionWrapper1<T, U>(map),
                            FunctionWrapper2<V, W &, X>(reduce),
                            options);
}

template <typename Sequence, typename MapFunctor, typename T, typename C, typename U, typename V, typename W>
QFuture<V> mappedReduced(const Sequence &sequence,
                         T (C::*map)() const,
                         U (reduce)(V &, W),
                         ReduceOptions options = SimpleReduce)
{
    return mappedReduced<V>(sequence,
                            ConstMemberFunctionWrapper<T, C>(map),
                            FunctionWrapper2<U, V &, W>(reduce),
                            options);
}

template <typename Sequence, typename T, typename U, typename V, typename C, typename W>
QFuture<C> mappedReduced(const Sequence &sequence,
                         T (map)(U),
                         V (C::*reduce)(W),
                         ReduceOptions options = SimpleReduce)
{
    return mappedReduced<C>(sequence,
                            FunctionWrapper1<T, U>(map),
                            MemberFunctionWrapper1<V, C, W>(reduce),
                            options);
}

template <typename Sequence, typename T, typename C, typename U,typename D, typename V>
QFuture<D> mappedReduced(const Sequence &sequence,
                         T (C::*map)() const,
                         U (D::*reduce)(V),
                         ReduceOptions options = SimpleReduce)
{
    return mappedReduced<D>(sequence,
                            ConstMemberFunctionWrapper<T, C>(map),
                            MemberFunctionWrapper1<U, D, V>(reduce),
                            options);
}

#if 0
// mappedReduced() with initial result value
template <typename ResultType, typename Sequence, typename MapFunctor, typename ReduceFunctor>
QFuture<ResultType> mappedReduced(ResultType initialValue,
                                  const Sequence &sequence,
                                  MapFunctor map,
                                  ReduceFunctor reduce,
                                  ReduceOptions options = SimpleReduce)
{
    if (options == OrderedReduce) {
        typedef OrderedReducer<ReduceFunctor, ResultType, typename MapFunctor::result_type> Reducer;
        const MappedReducedFor<ResultType, Sequence, MapFunctor, ReduceFunctor, Reducer>
            mapFor(initialValue, sequence, map, reduce);
        return QtConcurrent::runFor(0, sequence.size(), mapFor);
    }
    const MappedReducedFor<ResultType, Sequence, MapFunctor, ReduceFunctor>
        mapFor(initialValue, sequence, map, reduce);
    return QtConcurrent::runFor(0, sequence.size(), mapFor);
}

template <typename ResultType, typename Sequence, typename MapFunctor, typename T, typename U, typename V>
QFuture<ResultType> mappedReduced(ResultType initialValue,
                                  const Sequence &sequence,
                                  MapFunctor map,
                                  T (reduce)(U &, V),
                                  ReduceOptions options = SimpleReduce)
{
    return mappedReduced(initialValue,
                         sequence,
                         map,
                         FunctionWrapper2<T, U &, V>(reduce),
                         options);
}

template <typename ResultType, typename Sequence, typename MapFunctor, typename T, typename C, typename U>
QFuture<ResultType> mappedReduced(ResultType initialValue,
                                  const Sequence &sequence,
                                  MapFunctor map,
                                  T (C::*reduce)(U),
                                  ReduceOptions options = SimpleReduce)
{
    return mappedReduced(initialValue,
                         sequence,
                         map,
                         MemberFunctionWrapper1<T, C, U>(reduce),
                         options);
}

template <typename ResultType, typename Sequence, typename T, typename U, typename ReduceFunctor>
QFuture<ResultType> mappedReduced(ResultType initialValue,
                                  const Sequence &sequence,
                                  T (map)(U),
                                  ReduceFunctor reduce,
                                  ReduceOptions options = SimpleReduce)
{
    return mappedReduced(initialValue,
                         sequence,
                         FunctionWrapper1<T, U>(map),
                         reduce,
                         options);
}

template <typename ResultType, typename Sequence, typename T, typename C, typename ReduceFunctor>
QFuture<ResultType> mappedReduced(ResultType initialValue,
                                  const Sequence &sequence,
                                  T (C::*map)() const,
                                  ReduceFunctor reduce,
                                  ReduceOptions options = SimpleReduce)
{
    return mappedReduced(initialValue,
                         sequence,
                         ConstMemberFunctionWrapper<T, C>(map),
                         reduce,
                         options);
}

template <typename ResultType, typename Sequence, typename T, typename U, typename V, typename W, typename X>
QFuture<ResultType> mappedReduced(ResultType initialValue,
                                  const Sequence &sequence,
                                  T (map)(U),
                                  V (reduce)(W &, X),
                                  ReduceOptions options = SimpleReduce)
{
    return mappedReduced(initialValue,
                         sequence,
                         FunctionWrapper1<T, U>(map),
                         FunctionWrapper2<V, W &, X>(reduce),
                         options);
}

template <typename ResultType, typename Sequence, typename MapFunctor, typename T, typename C, typename U, typename V, typename W>
QFuture<ResultType> mappedReduced(ResultType initialValue,
                                  const Sequence &sequence,
                                  T (C::*map)() const,
                                  U (reduce)(V &, W),
                                  ReduceOptions options = SimpleReduce)
{
    return mappedReduced(initialValue,
                         sequence,
                         ConstMemberFunctionWrapper<T, C>(map),
                         FunctionWrapper2<U, V &, W>(reduce),
                         options);
}

template <typename ResultType, typename Sequence, typename T, typename U, typename V, typename C, typename W>
QFuture<ResultType> mappedReduced(ResultType initialValue,
                                  const Sequence &sequence,
                                  T (map)(U),
                                  V (C::*reduce)(W),
                                  ReduceOptions options = SimpleReduce)
{
    return mappedReduced(initialValue,
                         sequence,
                         FunctionWrapper1<T, U>(map),
                         MemberFunctionWrapper1<V, C, W>(reduce),
                         options);
}

template <typename ResultType, typename Sequence, typename T, typename C, typename U,typename D, typename V>
QFuture<ResultType> mappedReduced(ResultType initialValue,
                                  const Sequence &sequence,
                                  T (C::*map)() const,
                                  U (D::*reduce)(V),
                                  ReduceOptions options = SimpleReduce)
{
    return mappedReduced(initialValue,
                         sequence,
                         ConstMemberFunctionWrapper<T, C>(map),
                         MemberFunctionWrapper1<U, D, V>(reduce),
                         options);
}
#endif

// mapped()
template <typename OutputSequence, typename InputSequence, typename MapFunctor>
QFuture<OutputSequence> mapped(const InputSequence &sequence, MapFunctor map)
{
    return mappedReduced(sequence, map, &OutputSequence::append, OrderedReduce);
}

template <typename OutputSequence, typename InputSequence, typename T, typename U>
QFuture<OutputSequence> mapped(const InputSequence &sequence, T (map)(U))
{
    return mappedReduced(sequence, map, &OutputSequence::append, OrderedReduce);
}

template <typename OutputSequence, typename InputSequence, typename T, typename C>
QFuture<OutputSequence> mapped(const InputSequence &sequence, T (C::*map)() const)
{
    return mappedReduced(sequence, map, &OutputSequence::append, OrderedReduce);
}

template <typename Sequence, typename MapFunctor>
QFuture<Sequence> mapped(const Sequence &sequence, MapFunctor map)
{
    return mappedReduced(sequence, map, &Sequence::append, OrderedReduce);
}

template <typename Sequence, typename T, typename U>
QFuture<Sequence> mapped(const Sequence &sequence, T (map)(U))
{
    return mappedReduced(sequence, map, &Sequence::append, OrderedReduce);
}

template <typename Sequence, typename T, typename C>
QFuture<Sequence> mapped(const Sequence &sequence, T (C::*map)() const)
{
    return mappedReduced(sequence, map, &Sequence::append, OrderedReduce);
}




// Implementation of filter
template <typename Sequence, typename FilterFunctor, typename ReduceFunctor>
class FilterFor
{
    typedef OrderedReducer<ReduceFunctor, Sequence, typename Sequence::value_type> Reducer;

    Sequence reducedResult;
    Sequence &sequence;
    FilterFunctor filter;
    ReduceFunctor reduce;
    Reducer reducer;

public:
    FilterFor(Sequence &sequence, FilterFunctor filter, ReduceFunctor reduce)
        : reducedResult(), sequence(sequence), filter(filter), reduce(reduce)
    { }

    void runIterations(int begin, int end)
    {
        MapResults<typename Sequence::value_type> filterResults;
        filterResults.offset = begin;

        while (begin != end) {
            QSequenceItem<typename Sequence::value_type> item(sequence[begin], begin);
            if (!filter(item))
                filterResults.results.append(item.value());
            ++begin;
        }

        reducer.runReduce(reduce, reducedResult, filterResults);
    }

    void finish()
    {
        reducer.finish(reduce, reducedResult);
        sequence = reducedResult;
    }

    typedef void ResultType;
    void *result()
    {
        return 0;
    }
};

template <typename Sequence, typename FilterFunctor, typename T, typename C, typename U>
QFuture<void> filterInternal(Sequence &sequence, FilterFunctor filter, T (C::*reduce)(U))
{
    typedef MemberFunctionWrapper1<T, C, U> ReduceFunctor;
    const FilterFor<Sequence, FilterFunctor, ReduceFunctor> mapFor(sequence, filter, ReduceFunctor(reduce));
    return QtConcurrent::runFor(0, sequence.size(), mapFor);
}

// filter()
template <typename Sequence, typename FilterFunctor>
QFuture<void> filter(Sequence &sequence, FilterFunctor filter)
{
    return filterInternal(sequence, filter, &Sequence::append);
}

template <typename Sequence, typename T>
QFuture<void> filter(Sequence &sequence, bool (filter)(T))
{
    return filterInternal(sequence, FunctionWrapper1<bool, T>(filter), &Sequence::append);
}

template <typename Sequence, typename C>
QFuture<void> filter(Sequence &sequence, bool (C::*filter)() const)
{
    return filterInternal(sequence, ConstMemberFunctionWrapper<bool, C>(filter), &Sequence::append);
}




// Implementation of filter-reduce
template <typename ReducedResultType,
          typename Sequence,
          typename FilterFunctor,
          typename ReduceFunctor,
          typename Reducer = SimpleReducer<ReduceFunctor, ReducedResultType, typename Sequence::value_type> >
class FilteredReducedFor
{
    ReducedResultType reducedResult;
    const Sequence sequence;
    FilterFunctor filter;
    ReduceFunctor reduce;
    Reducer reducer;

public:
    FilteredReducedFor(const Sequence &sequence, FilterFunctor filter, ReduceFunctor reduce)
        : reducedResult(), sequence(sequence), filter(filter), reduce(reduce)
    { }

    FilteredReducedFor(ReducedResultType r,
                       const Sequence &sequence,
                       FilterFunctor filter,
                       ReduceFunctor reduce)
        : reducedResult(r), sequence(sequence), filter(filter), reduce(reduce)
    { }

    void runIterations(int begin, int end)
    {
        MapResults<typename Sequence::value_type> filterResults;
        filterResults.offset = begin;

        while (begin != end) {
            QSequenceItem<typename Sequence::value_type> item(sequence[begin], begin);
            if (!filter(item))
                filterResults.results.append(item.value());
            ++begin;
        }

        reducer.runReduce(reduce, reducedResult, filterResults);
    }

    void finish()
    {
        reducer.finish(reduce, reducedResult);
    }

    typedef ReducedResultType ResultType;
    ReducedResultType *result()
    {
        return &reducedResult;
    }
};

// filteredReduced()
template <typename ResultType, typename Sequence, typename FilterFunctor, typename ReduceFunctor>
QFuture<ResultType> filteredReduced(const Sequence &sequence,
                                    FilterFunctor filter,
                                    ReduceFunctor reduce,
                                    ReduceOptions options = SimpleReduce)
{
    if (options == OrderedReduce) {
        typedef OrderedReducer<ReduceFunctor, ResultType, typename Sequence::value_type> Reducer;
        const FilteredReducedFor<ResultType, Sequence, FilterFunctor, ReduceFunctor, Reducer>
            mapFor(sequence, filter, reduce);
        return QtConcurrent::runFor(0, sequence.size(), mapFor);
    }
    const FilteredReducedFor<ResultType, Sequence, FilterFunctor, ReduceFunctor>
        mapFor(sequence, filter, reduce);
    return QtConcurrent::runFor(0, sequence.size(), mapFor);
}

template <typename ResultType, typename Sequence, typename T, typename ReduceFunctor>
QFuture<ResultType> filteredReduced(const Sequence &sequence,
                                    bool (filter)(T),
                                    ReduceFunctor reduce,
                                    ReduceOptions options = SimpleReduce)
{
    return filteredReduced<ResultType>(sequence,
                                       FunctionWrapper1<bool, T>(filter),
                                       reduce,
                                       options);
}

template <typename ResultType, typename Sequence, typename C, typename ReduceFunctor>
QFuture<ResultType> filteredReduced(const Sequence &sequence,
                                    bool (C::*filter)() const,
                                    ReduceFunctor reduce,
                                    ReduceOptions options = SimpleReduce)
{
    return filteredReduced<ResultType>(sequence,
                                       ConstMemberFunctionWrapper<bool, C>(filter),
                                       reduce,
                                       options);
}

template <typename Sequence, typename FilterFunctor, typename T, typename U, typename V>
QFuture<U> filteredReduced(const Sequence &sequence,
                           FilterFunctor filter,
                           T (reduce)(U &, V),
                           ReduceOptions options = SimpleReduce)
{
    return filteredReduced<U>(sequence,
                              filter,
                              FunctionWrapper2<T, U &, V>(reduce),
                              options);
}

template <typename Sequence, typename FilterFunctor, typename T, typename C, typename U>
QFuture<C> filteredReduced(const Sequence &sequence,
                           FilterFunctor filter,
                           T (C::*reduce)(U),
                           ReduceOptions options = SimpleReduce)
{
    return filteredReduced<C>(sequence,
                              filter,
                              MemberFunctionWrapper1<T, C, U>(reduce),
                              options);
}

template <typename Sequence, typename T, typename U, typename V, typename W>
QFuture<V> filteredReduced(const Sequence &sequence,
                           bool (filter)(T),
                           U (reduce)(V &, W),
                           ReduceOptions options = SimpleReduce)
{
    return filteredReduced<V>(sequence,
                              FunctionWrapper1<bool, T>(filter),
                              FunctionWrapper2<U, V &, W>(reduce),
                              options);
}

template <typename Sequence, typename C, typename T, typename U, typename V>
QFuture<U> filteredReduced(const Sequence &sequence,
                           bool (C::*filter)() const,
                           T (reduce)(U &, V),
                           ReduceOptions options = SimpleReduce)
{
    return filteredReduced<U>(sequence,
                              ConstMemberFunctionWrapper<bool, C>(filter),
                              FunctionWrapper2<T, U &, V>(reduce),
                              options);
}

template <typename Sequence, typename T, typename U, typename C, typename V>
QFuture<C> filteredReduced(const Sequence &sequence,
                           bool (filter)(T),
                           U (C::*reduce)(V),
                           ReduceOptions options = SimpleReduce)
{
    return filteredReduced<C>(sequence,
                              FunctionWrapper1<bool, T>(filter),
                              MemberFunctionWrapper1<U, C, V>(reduce),
                              options);
}

template <typename Sequence, typename C, typename T, typename D, typename U>
QFuture<D> filteredReduced(const Sequence &sequence,
                           bool (C::*filter)() const,
                           T (D::*reduce)(U),
                           ReduceOptions options = SimpleReduce)
{
    return filteredReduced<D>(sequence,
                              ConstMemberFunctionWrapper<bool, C>(filter),
                              MemberFunctionWrapper1<T, D, U>(reduce),
                              options);
}

#if 0
// filteredReduced() with initial result value
template <typename ResultType, typename Sequence, typename FilterFunctor, typename ReduceFunctor>
QFuture<ResultType> filteredReduced(ResultType initialValue,
                                    const Sequence &sequence,
                                    FilterFunctor filter,
                                    ReduceFunctor reduce,
                                    ReduceOptions options = SimpleReduce)
{
    if (options == OrderedReduce) {
        typedef OrderedReducer<ReduceFunctor, ResultType, typename Sequence::value_type> Reducer;
        const FilteredReducedFor<ResultType, Sequence, FilterFunctor, ReduceFunctor, Reducer>
            mapFor(initialValue, sequence, filter, reduce);
        return QtConcurrent::runFor(0, sequence.size(), mapFor);
    }
    const FilteredReducedFor<ResultType, Sequence, FilterFunctor, ReduceFunctor>
        mapFor(initialValue, sequence, filter, reduce);
    return QtConcurrent::runFor(0, sequence.size(), mapFor);
}

template <typename ResultType, typename Sequence, typename T, typename ReduceFunctor>
QFuture<ResultType> filteredReduced(ResultType initialValue,
                                    const Sequence &sequence,
                                    bool (filter)(T),
                                    ReduceFunctor reduce,
                                    ReduceOptions options = SimpleReduce)
{
    return filteredReduced(initialValue,
                           sequence,
                           FunctionWrapper1<bool, T>(filter),
                           reduce,
                           options);
}

template <typename ResultType, typename Sequence, typename C, typename ReduceFunctor>
QFuture<ResultType> filteredReduced(ResultType initialValue,
                                    const Sequence &sequence,
                                    bool (C::*filter)() const,
                                    ReduceFunctor reduce,
                                    ReduceOptions options = SimpleReduce)
{
    return filteredReduced(initialValue,
                           sequence,
                           ConstMemberFunctionWrapper<bool, C>(filter),
                           reduce,
                           options);
}

template <typename ResultType, typename Sequence, typename FilterFunctor, typename T, typename U, typename V>
QFuture<ResultType> filteredReduced(ResultType initalValue,
                                    const Sequence &sequence,
                                    FilterFunctor filter,
                                    T (reduce)(U &, V),
                                    ReduceOptions options = SimpleReduce)
{
    return filteredReduced(initialValue,
                           sequence,
                           filter,
                           FunctionWrapper2<T, U &, V>(reduce),
                           options);
}

template <typename ResultType, typename Sequence, typename FilterFunctor, typename T, typename C, typename U>
QFuture<ResultType> filteredReduced(ResultType initialValue,
                                    const Sequence &sequence,
                                    FilterFunctor filter,
                                    T (C::*reduce)(U),
                                    ReduceOptions options = SimpleReduce)
{
    return filteredReduced(initialValue,
                           sequence,
                           filter,
                           MemberFunctionWrapper1<T, C, U>(reduce),
                           options);
}

template <typename ResultType, typename Sequence, typename T, typename U, typename V, typename W>
QFuture<ResultType> filteredReduced(ResultType initialValue,
                                    const Sequence &sequence,
                           bool (filter)(T),
                           U (reduce)(V &, W),
                           ReduceOptions options = SimpleReduce)
{
    return filteredReduced(initialValue,
                           sequence,
                           FunctionWrapper1<bool, T>(filter),
                           FunctionWrapper2<U, V &, W>(reduce),
                           options);
}

template <typename ResultType, typename Sequence, typename C, typename T, typename U, typename V>
QFuture<ResultType> filteredReduced(ResultType initialValue,
                                    const Sequence &sequence,
                                    bool (C::*filter)() const,
                                    T (reduce)(U &, V),
                                    ReduceOptions options = SimpleReduce)
{
    return filteredReduced(initialValue,
                           sequence,
                           ConstMemberFunctionWrapper<bool, C>(filter),
                           FunctionWrapper2<T, U &, V>(reduce),
                           options);
}

template <typename ResultType, typename Sequence, typename T, typename U, typename C, typename V>
QFuture<ResultType> filteredReduced(ResultType initialValue,
                                    const Sequence &sequence,
                                    bool (filter)(T),
                                    U (C::*reduce)(V),
                                    ReduceOptions options = SimpleReduce)
{
    return filteredReduced<(initialValue,
                            sequence,
                            FunctionWrapper1<bool, T>(filter),
                            MemberFunctionWrapper1<U, C, V>(reduce),
                            options);
}

template <typename ResultType, typename Sequence, typename C, typename T, typename D, typename U>
QFuture<ResultType> filteredReduced(ResultType initialValue,
                                    const Sequence &sequence,
                                    bool (C::*filter)() const,
                                    T (D::*reduce)(U),
                                    ReduceOptions options = SimpleReduce)
{
    return filteredReduced(initialValue,
                           sequence,
                           ConstMemberFunctionWrapper<bool, C>(filter),
                           MemberFunctionWrapper1<T, D, U>(reduce),
                           options);
}
#endif

// filtered()
template <typename Sequence, typename FilterFunctor>
QFuture<Sequence> filtered(const Sequence &sequence, FilterFunctor filter)
{
    return filteredReduced(sequence, filter, &Sequence::append, OrderedReduce);
}

template <typename Sequence, typename T>
QFuture<Sequence> filtered(const Sequence &sequence, bool (filter)(T))
{
    return filteredReduced(sequence, filter, &Sequence::append, OrderedReduce);
}

template <typename Sequence, typename C>
QFuture<Sequence> filtered(const Sequence &sequence, bool (C::*filter)() const)
{
    return filteredReduced(sequence, filter, &Sequence::append, OrderedReduce);
}

} // namespace QtConcurrent

#endif // QTCONCURRET_MAP_H
