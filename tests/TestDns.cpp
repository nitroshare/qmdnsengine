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

#include <QMap>
#include <QObject>
#include <QTest>

#include <qmdnsengine/dns.h>
#include <qmdnsengine/record.h>

#define PARSE_RECORD() \
    quint16 offset = 0; \
    QMdnsEngine::Record record; \
    bool result = QMdnsEngine::parseRecord( \
        QByteArray(packetData, sizeof(packetData)), \
        offset, \
        record \
    )

class TestDns : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testParseName_data();
    void testParseName();

    void testParseRecordA();
    void testParseRecordAAAA();
    void testParseRecordPTR();
    void testParseRecordSRV();
    void testParseRecordTXT();
};

void TestDns::testParseName_data()
{
    QTest::addColumn<QByteArray>("packet");
    QTest::addColumn<quint16>("initialOffset");
    QTest::addColumn<quint16>("correctOffset");
    QTest::addColumn<QByteArray>("correctName");
    QTest::addColumn<bool>("correctResult");

    const char simpleData[] = {
        '\x04', '_', 't', 'c', 'p',
        '\x05', 'l', 'o', 'c', 'a', 'l',
        '\0'
    };
    QTest::newRow("simple")
            << QByteArray(simpleData, sizeof(simpleData))
            << static_cast<quint16>(0)
            << static_cast<quint16>(12)
            << QByteArray("_tcp.local.")
            << true;

    const char pointerData[] = {
        '\x04', '_', 't', 'c', 'p',
        '\x05', 'l', 'o', 'c', 'a', 'l',
        '\0',
        '\x04', 't', 'e', 's', 't',
        '\xc0', '\0'
    };
    QTest::newRow("pointer")
            << QByteArray(pointerData, sizeof(pointerData))
            << static_cast<quint16>(12)
            << static_cast<quint16>(19)
            << QByteArray("test._tcp.local.")
            << true;

    const char corruptData[] = {
        '\x03', '1', '2'
    };
    QTest::newRow("corrupt data")
            << QByteArray(corruptData, sizeof(corruptData))
            << static_cast<quint16>(0)
            << static_cast<quint16>(0)
            << QByteArray()
            << false;
}

void TestDns::testParseName()
{
    QFETCH(QByteArray, packet);
    QFETCH(quint16, initialOffset);
    QFETCH(quint16, correctOffset);
    QFETCH(QByteArray, correctName);
    QFETCH(bool, correctResult);

    quint16 offset = initialOffset;
    QByteArray name;
    bool result = QMdnsEngine::parseName(packet, offset, name);

    QCOMPARE(result, correctResult);
    if (result) {
        QCOMPARE(offset, correctOffset);
        QCOMPARE(name, correctName);
    }
}

void TestDns::testParseRecordA()
{
    const char packetData[] = {
        '\x04', 't', 'e', 's', 't', '\0',
        '\x00', '\x01',
        '\x80', '\x00',
        '\x00', '\x00', '\x0e', '\x10',
        '\x00', '\x04',
        '\x7f', '\x00', '\x00', '\x01'
    };

    PARSE_RECORD();

    QCOMPARE(result, true);
    QCOMPARE(record.name(), QByteArray("test."));
    QCOMPARE(record.type(), static_cast<quint16>(QMdnsEngine::A));
    QCOMPARE(record.flushCache(), true);
    QCOMPARE(record.ttl(), static_cast<quint32>(3600));
    QCOMPARE(record.address(), QHostAddress("127.0.0.1"));
}

void TestDns::testParseRecordAAAA()
{
    const char packetData[] = {
        '\x04', 't', 'e', 's', 't', '\0',
        '\x00', '\x1c',
        '\x00', '\x00',
        '\x00', '\x00', '\x00', '\x00',
        '\x00', '\x10',
        '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x01'
    };

    PARSE_RECORD();

    QCOMPARE(result, true);
    QCOMPARE(record.type(), static_cast<quint16>(QMdnsEngine::AAAA));
    QCOMPARE(record.address(), QHostAddress("::1"));
}

void TestDns::testParseRecordPTR()
{
    const char packetData[] = {
        '\x04', 't', 'e', 's', 't', '\0',
        '\x00', '\x0c',
        '\x00', '\x00',
        '\x00', '\x00', '\x00', '\x00',
        '\x00', '\x07',
        '\x05', 't', 'e', 's', 't', '2', '\0',
    };

    PARSE_RECORD();

    QCOMPARE(result, true);
    QCOMPARE(record.type(), static_cast<quint16>(QMdnsEngine::PTR));
    QCOMPARE(record.target(), QByteArray("test2."));
}

void TestDns::testParseRecordSRV()
{
    const char packetData[] = {
        '\x04', 't', 'e', 's', 't', '\0',
        '\x00', '\x21',
        '\x00', '\x00',
        '\x00', '\x00', '\x00', '\x00',
        '\x00', '\x0d',
        '\x00', '\x01', '\x00', '\x02', '\x00', '\x03',
        '\x05', 't', 'e', 's', 't', '2', '\0',
    };

    PARSE_RECORD();

    QCOMPARE(result, true);
    QCOMPARE(record.type(), static_cast<quint16>(QMdnsEngine::SRV));
    QCOMPARE(record.priority(), static_cast<quint16>(1));
    QCOMPARE(record.weight(), static_cast<quint16>(2));
    QCOMPARE(record.port(), static_cast<quint16>(3));
    QCOMPARE(record.target(), QByteArray("test2."));
}

void TestDns::testParseRecordTXT()
{
    const char packetData[] = {
        '\x04', 't', 'e', 's', 't', '\0',
        '\x00', '\x10',
        '\x00', '\x00',
        '\x00', '\x00', '\x00', '\x00',
        '\x00', '\x08',
        '\x03', 'a', '=', 'a',
        '\x03', 'b', '=', 'b'
    };

    PARSE_RECORD();

    QMap<QByteArray, QByteArray> attributes{
        {"a", "a"},
        {"b", "b"}
    };

    QCOMPARE(result, true);
    QCOMPARE(record.type(), static_cast<quint16>(QMdnsEngine::TXT));
    QCOMPARE(record.attributes(), attributes);
}

QTEST_MAIN(TestDns)
#include "TestDns.moc"
