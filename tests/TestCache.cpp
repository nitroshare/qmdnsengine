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

#include <QObject>
#include <QSignalSpy>
#include <QTest>

#include <qmdnsengine/dns.h>
#include <qmdnsengine/cache.h>
#include <qmdnsengine/record.h>

Q_DECLARE_METATYPE(QMdnsEngine::Record)

class TestCache : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase();
    void testExpiry();
    void testRemoval();
};

void TestCache::initTestCase()
{
    qRegisterMetaType<QMdnsEngine::Record>("Record");
}

void TestCache::testExpiry()
{
    QMdnsEngine::Record record;
    record.setName("Test");
    record.setType(QMdnsEngine::A);
    record.setTtl(1);

    QMdnsEngine::Cache cache;
    cache.addRecord(record);

    QSignalSpy shouldQuerySpy(&cache, SIGNAL(shouldQuery(Record)));
    QSignalSpy recordExpiredSpy(&cache, SIGNAL(recordExpired(Record)));

    // The record should be in the cache
    QVERIFY(cache.lookupRecord(record.name(), record.type(), record));

    // This unfortunately delays the test but 1s is the smallest TTL
    QTest::qWait(1100);

    // After entering the event loop, the record should have been purged and
    // the recordExpired() signal emitted
    QVERIFY(!cache.lookupRecord(record.name(), record.type(), record));
    QVERIFY(shouldQuerySpy.count() > 0);
    QCOMPARE(recordExpiredSpy.count(), 1);
}

void TestCache::testRemoval()
{
    QMdnsEngine::Record record;
    record.setName("Test");
    record.setType(QMdnsEngine::A);
    record.setTtl(10);

    QMdnsEngine::Cache cache;
    cache.addRecord(record);

    QSignalSpy recordExpiredSpy(&cache, SIGNAL(recordExpired(Record)));

    // Purge the record from the cache by setting its TTL to 0
    record.setTtl(0);
    cache.addRecord(record);

    // Verify that the record is gone
    QVERIFY(!cache.lookupRecord(record.name(), record.type(), record));
    QCOMPARE(recordExpiredSpy.count(), 1);
}

QTEST_MAIN(TestCache)
#include "TestCache.moc"
