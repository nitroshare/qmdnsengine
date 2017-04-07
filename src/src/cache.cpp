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

#include <qmdnsengine/cache.h>

#include "cache_p.h"

using namespace QMdnsEngine;

bool QMdnsEngine::operator<(const CacheKey &key1, const CacheKey &key2)
{
    return key1.type == key2.type ?
        key1.name < key2.name :
        key1.type < key2.type;
}

CachePrivate::CachePrivate(QObject *parent)
    : QObject(parent)
{
    connect(&mTimer, &QTimer::timeout, this, &CachePrivate::onTimeout);

    mTimer.setSingleShot(true);
}

void CachePrivate::onTimeout()
{
    // Filter out expired entries and find the next earliest expiry
    QDateTime now = QDateTime::currentDateTime();
    QDateTime nextExpiry;
    for (auto i = mRecords.begin(); i != mRecords.end(); ++i) {
        if (i.value().expiry <= now) {
            i = mRecords.erase(i);
        } else {
            nextExpiry = qMin(i.value().expiry, nextExpiry);
        }
    }

    // Set the timer for the point when the next record expires
    mNextExpiry = nextExpiry;
    if (mNextExpiry.isValid()) {
        mTimer.start(now.msecsTo(nextExpiry));
    }
}

Cache::Cache(QObject *parent)
    : QObject(parent),
      d(new CachePrivate(this))
{
}

void Cache::addRecord(const Record &record, const QDateTime &now)
{
    QDateTime expiry = now.addSecs(record.ttl());
    d->mRecords.insert(
        {record.name(), record.type()},
        {expiry, record}
    );

    // Determine if the record expires sooner than the next one
    if (expiry < d->mNextExpiry) {
        d->mTimer.stop();
        d->mTimer.start(now.msecsTo(expiry));
    }
}

bool Cache::lookup(const QByteArray &name, quint16 type, Record &record)
{
    CacheValue value = d->mRecords.value({name, type});
    if (value.expiry.isValid()) {
        record = value.record;
        return true;
    }
    return false;
}
