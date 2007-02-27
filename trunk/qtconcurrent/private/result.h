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
#ifndef QTCONCURRENT_RESULT_H
#define QTCONCURRENT_RESULT_H

namespace QtConcurrent {

template <typename DataType>
class Result {
public:
    // Constructs a valid result.
    Result(const DataType &data)
    : m_data(data), valid(true) {}

    // Construct an invalid result.
    Result()
    : m_data(DataType()), valid(false) {}

    // Returns wether the result is valid
    inline bool isValid() const
    { return valid; }

    // Returns the value for the result.
    inline DataType data() const
    { return m_data; }
private:
    DataType m_data;
    bool valid;
};

} // namespace QtConcurrent

#endif
