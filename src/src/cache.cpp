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

CachePrivate::CachePrivate(Cache *cache)
    : QObject(cache),
      q(cache)
{
    connect(&timer, &QTimer::timeout, this, &CachePrivate::onTimeout);

    timer.setSingleShot(true);
}

void CachePrivate::onTimeout()
{
    QDateTime now = QDateTime::currentDateTime();

    // Loop through each of the entries, processing the triggers and searching
    // for the next earliest trigger
    QDateTime newNextTrigger;
    for (auto i = entries.begin(); i != entries.end();) {
        auto &triggers = (*i).triggers;

        // Loop through the triggers remaining in the entry, removing ones
        // that have already passed
        bool shouldQuery = false;
        for (auto j = triggers.begin(); j != triggers.end();) {
            if ((*j) <= now) {
                shouldQuery = true;
                j = triggers.erase(j);
            } else {
                ++j;
            }
        }

        // If any triggers remain, determine if they are the next earliest; if
        // not, remove the entry and indicate the record expired
        if (triggers.length()) {
            if (newNextTrigger.isNull() || triggers.at(0) < newNextTrigger) {
                newNextTrigger = triggers.at(0);
            }
            if (shouldQuery) {
                emit q->shouldQuery((*i).record);
            }
            ++i;
        } else {
            emit q->recordExpired((*i).record);
            i = entries.erase(i);
        }
    }

    // If newNextTrigger contains a value, it will be the time for the next
    // trigger and the timer should be started again
    nextTrigger = newNextTrigger;
    if (!nextTrigger.isNull()) {
        timer.start(now.msecsTo(nextTrigger));
    }
}

Cache::Cache(QObject *parent)
    : QObject(parent),
      d(new CachePrivate(this))
{
}

void Cache::addRecord(const Record &record)
{
    // Any record with the flush cache bit set should cause all other records
    // with the same name and type to be removed; any record with a TTL of
    // zero should be immediately removed
    if (record.flushCache() || record.ttl() == 0) {
        for (auto i = d->entries.begin(); i != d->entries.end();) {
            if (record.flushCache() &&
                    (*i).record.name() == record.name() &&
                    (*i).record.type() == record.type() ||
                    !record.flushCache() && (*i).record == record) {
                emit recordExpired((*i).record);
                i = d->entries.erase(i);
            } else {
                ++i;
            }
        }
        if (!record.flushCache()) {
            return;
        }
    }

    QDateTime now = QDateTime::currentDateTime();

    // TODO: add some random variation to these values

    // Create triggers for the record
    QList<QDateTime> triggers{
        now.addMSecs(record.ttl() * 500),  // 50%
        now.addMSecs(record.ttl() * 850),  // 85%
        now.addMSecs(record.ttl() * 900),  // 90%
        now.addMSecs(record.ttl() * 950),  // 95%
        now.addSecs(record.ttl())
    };

    // Loop through the existing entries, looking for a matching entry
    auto before = d->entries.end();
    for (auto i = d->entries.begin(); i != d->entries.end(); ++i) {
        if ((*i).record == record) {
            before = d->entries.erase(i);
            break;
        }
    }

    // Insert the record and its triggers
    d->entries.insert(before, {record, triggers});

    // Check if half of this record's lifetime is earlier than the next
    // scheduled trigger; if so, restart the timer
    if (d->nextTrigger.isNull() || triggers.at(0) < d->nextTrigger) {
        d->timer.stop();
        d->timer.start(now.msecsTo(triggers.at(0)));
    }
}

bool Cache::lookupRecord(const QByteArray &name, quint16 type, Record &record)
{
    QList<Record> records;
    if (lookupRecords(name, type, records)) {
        record = records.at(0);
        return true;
    }
    return false;
}

bool Cache::lookupRecords(const QByteArray &name, quint16 type, QList<Record> &records)
{
    foreach (CachePrivate::Entry entry, d->entries) {
        if ((name.isNull() || entry.record.name() == name) && entry.record.type() == type) {
            records.append(entry.record);
        }
    }
    return records.length();
}
