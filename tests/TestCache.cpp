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

#include <QDateTime>
#include <QObject>
#include <QTest>

#include <qmdnsengine/dns.h>
#include <qmdnsengine/cache.h>
#include <qmdnsengine/record.h>

class TestCache : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testExpiry();
};

void TestCache::testExpiry()
{
    QDateTime now = QDateTime::currentDateTime().addSecs(-1);

    QMdnsEngine::Record record;
    record.setName("Test");
    record.setType(QMdnsEngine::A);
    record.setTtl(1);

    QMdnsEngine::Cache cache;
    cache.addRecord(record, now);

    // The record should be in the cache
    QVERIFY(cache.lookup(record.name(), record.type(), record));

    // After entering the event loop, the record should not be purged
    QTRY_VERIFY(!cache.lookup(record.name(), record.type(), record));
}

QTEST_MAIN(TestCache)
#include "TestCache.moc"
