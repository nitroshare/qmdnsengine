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
#include <QTest>

#include <qmdnsengine/dns.h>

class TestDns : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testParseName_data();
    void testParseName();
};

void TestDns::testParseName_data()
{
    QTest::addColumn<QByteArray>("packet");
    QTest::addColumn<quint16>("initialOffset");
    QTest::addColumn<QByteArray>("correctName");
    QTest::addColumn<bool>("correctResult");

    const char simpleData[] = {
        '\x04', '_', 't', 'c', 'p',
        '\x05', 'l', 'o', 'c', 'a', 'l',
        '\0'
    };
    QTest::newRow("simple")
            << QByteArray(simpleData, sizeof(simpleData))
            << 0
            << QByteArray("_tcp.local.")
            << true;
}

void TestDns::testParseName()
{
    QFETCH(QByteArray, packet);
    QFETCH(quint16, initialOffset);
    QFETCH(QByteArray, correctName);
    QFETCH(bool, correctResult);

    quint16 offset = initialOffset;
    QByteArray name;
    bool result = QMdnsEngine::parseName(packet, offset, name);

    QCOMPARE(result, correctResult);
    if (result) {
        QCOMPARE(offset - initialOffset, static_cast<quint16>(packet.length()));
        QCOMPARE(name, correctName);
    }
}

QTEST_MAIN(TestDns)
#include "TestDns.moc"
