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

#include <qmdnsengine/cache.h>

const QByteArray Name("name");
const QByteArray NameInvalid("name2");

// Test item used for storage in QVariant
class TestItem
{
public:
    QByteArray value1;
    quint16 value2;
};

Q_DECLARE_METATYPE(TestItem)

bool operator==(const TestItem &item1, const TestItem &item2)
{
    return item1.value1 == item2.value1 && item1.value2 == item2.value2;
}

bool operator<(const TestItem &item1, const TestItem &item2)
{
    return item1.value2 == item2.value2 ?
        item1.value1 < item1.value1 : item1.value2 < item2.value2;
}

const TestItem Item{"item", 42};

class TestCache : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase();
    void testExpiry();
};

void TestCache::initTestCase()
{
    QMetaType::registerComparators<TestItem>();
}

void TestCache::testExpiry()
{
    QMdnsEngine::Cache cache;
    cache.addItem(Name, QVariant::fromValue(Item), 0);

    // The item should be in the cache and invalid names should fail
    QCOMPARE(cache.lookup(Name), QVariant::fromValue(Item));
    QCOMPARE(cache.lookup(NameInvalid), QVariant());

    QSignalSpy itemExpiredSpy(&cache, SIGNAL(itemExpired(QVariant,QVariant)));
    QTest::qWait(0);

    // After entering the event loop, the item should have been purged and
    // the itemExpired() signal emitted
    QCOMPARE(cache.lookup(Name), QVariant());
    QCOMPARE(itemExpiredSpy.count(), 1);
}

QTEST_MAIN(TestCache)
#include "TestCache.moc"
