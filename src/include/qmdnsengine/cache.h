/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Nathan Osman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef QMDNSENGINE_CACHE_H
#define QMDNSENGINE_CACHE_H

#include <QDateTime>
#include <QObject>

#include "qmdnsengine_export.h"

namespace QMdnsEngine
{

class Record;

class QMDNSENGINE_EXPORT CachePrivate;

/**
 * @brief Cache for DNS records
 *
 * Records are stored in the cache until they are considered to have expired,
 * at which point they are purged.
 */
class QMDNSENGINE_EXPORT Cache : public QObject
{
    Q_OBJECT

public:

    explicit Cache(QObject *parent = 0);

    /**
     * @brief Add a record to the cache
     * @param record add this record to the cache
     * @param now time to calculate TTL relative to
     *
     * The TTL for the record will be added to the provided time to calculate
     * when the record expires.
     */
    void addRecord(const Record &record, const QDateTime &now = QDateTime::currentDateTime());

    /**
     * @brief Retrieve a record from the cache
     * @param name name of record to retrieve
     * @param type type of record to retrieve
     * @param record storage for the record retrieved
     * @return true if the record was retrieved
     */
    bool lookup(const QByteArray &name, quint16 type, Record &record);

Q_SIGNALS:

    /**
     * @brief Indicate that the specified record expired
     */
    void recordExpired(const Record &record);

private:

    CachePrivate *const d;
};

}

#endif // QMDNSENGINE_CACHE_H
