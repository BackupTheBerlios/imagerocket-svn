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
    \headerfile <map.h>
    \title Map and MapReduce
    
    \brief mapped() and mappedReduced() runs computations in parallel on the items in a sequence
    such as a QList or a QVector. 
    
    mapped() returns a new list containing the modified content,  while mappedReduced() returns a single result.

    \section1 mapped()
    mappped() takes an input list and a map function. This map function
    is then called for each item in the list, and a new list containing
    the return values from the map function is returned.
           
    The map function should be declared like this:
    \code
    T function(const U &u);
    \endcode
    T and U can be any type (and they can be the same type), but U must match the type
    stored in the list. If you want to use a function that takes more than one argument,
    you can use the bind function to create a function that map can use.
    
    This example shows how to apply a scale function to all the items in a list:
    \code
    QImage scale(const QImage &image)
    {
        return image.scaled(100, 100);
    }

    QList<QImage> images = ...;
    QList<QImage> thumbnails = mapped(images, scale);
    \endcode        

    It's also possible to assign the return value from mapped() to a QFuture:
    \code
    QImage scale(const QImage &image)
    {
        return image.scaled(100, 100);
    }
    
    QList<QImage> images = ...;
    QFuture<QList<QImage> > future = mapped(images, scale);
    \endcode

    This makes mapped() an asychronous function call. See the QFuture and QFutureWatcher
    documentation for more information.
    
    \section2 Sequence Items

    QSequenceItem and QMutableSequenceItem provides progress information that can be
    used by the map function. QSequenceItem is used with mapped(), and QMutableSequenceItem
    is used with map(). Use QSequenceItem like this:
    \code
    QImage scale(const QSequenceItem<QImage> &it)
    {
        qDebug() << "processing image" << it.index() << "of" << it.itemCount();
        const QImage image = it.data();
        return image.scaled(100, 100);
    }
    \endcode

    \section2 Member functions

    mapped() also accepts pointers to member functions. The member
    function class type must match the type stored in the list.

    \code
    
    QList<QImage> images = ...;
    QList<QImage> bgrImages = mapped(images, &QImage::rgbSwapped());

    \endcode

    \section2 Using bind() to bind function arguments
    
    If you want to use a map function takes more than one argument you can
    use bind() to transform it onto a function that takes one
    argument.
    
    As an example, we'll use QImage::scaledToWidth():

    \code
    QImage QImage::scaledToWidth(int width, Qt::TransformationMode) const;
    \endcode
        
      
    scaledToWidth takes three arguments (including the "this" pointer) and 
    can't be used with mapped()  directly, because mapped() expects a function
    that takes one argument.  To use this function with mapped() we have to 
    provide a value for \e{width} and \e{transformation mode}:

    \code
    bind(&QImage::scaledToWidth, 100 Qt::SmoothTransformation)
    \endcode

    The return value from bind() is a function object (functor) with
    the following signature: 
    \code
    QImage scaledToWith(const QImage &image)
    \endcode

    This matches what mapped() expects, and the complete example becomes:
    \code
   
    QList<QImage> images = ...;
    QList<QImage> thumbnails = mapped(images, bind(&QImage::scaledToWidth, 100 Qt::SmoothTransformation));

    \endcode

    \section1 map()
    If you want to modify a list in-place, use map(). The map function
    must then be of the form: 

    \code
    void function(U &u);
    \endcode

    Using map() is similar to using mapped():

    \code
    QImage scale(const QImage &image)
    {
        image = image.scaled(100, 100);
    }

    QList<QImage> images;
    QFuture<void> future = map(images, scale);
    \endcode        

    Since the return type ofmap() is void, we get a QFuture<void> back. If you
    ignore the return value, the call will block until the result is ready:

    \code
    QList<QImage> images;
    map(images, scale); // will block
    \endcode        
    
    \section1 MapReduce

    mappedReduced() is similar to mapped(), but instead of returning a list with the new
    results, the results are combined into a single value using a reduce function.
    
    Call mappedReduced like this:
    \code 
        Result result = mappedReducedd(list, mapFunction, reduceFunction);
    \endcode

    The reduce function should be declared like this:
    
    \code 
        T function (T &result, const U &mapResult)
    \endcode    

    The reduce function will be called once for each result returned by the map
    function, and should merge the \e{mapResult} into the \e{result} varible.
    mappedReduced() guarantees that only one thread will call reduce at a time, 
    so using a mutex to lock the result variable is not neccesary.

    See also the word count and compress examples.
*/


/*!
    \fn QFuture map(const Sequence &sequence, MapFunction function)
    \relates <map.h>
    Calls \a function once for each item in the \a sequence. The \a
    function is passed a reference to the item, so any modifications
    done to the item will appear in the \a sequence.
*/

/*!
    \fn QFuture mapped(const Sequence &sequence, MapFunction function)
    \relates <map.h>
    Calls \a function once for each item in the \a sequence and returns a
    Sequence of all mapped items.
*/

/*!
   \fn QFuture mappedReduced(const Sequence &sequence, MapFunction mapFunction, ReduceFunction reduceFunction)
    \relates <map.h>
    Calls \a mapFunction once for each item in the \a sequence. The return
    value of each \a mapFunction is passed to \a reduceFunction.

    Note that while \a mapFunction is called concurrently, only one thread
    at a time will call \a reduceFunction. The order in which \a reduceFunction is
    called is undefined.
*/
