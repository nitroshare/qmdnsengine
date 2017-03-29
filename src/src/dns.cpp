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

#include <QHostAddress>
#include <QtEndian>

#include <qmdnsengine/bitmap.h>
#include <qmdnsengine/dns.h>
#include <qmdnsengine/message.h>
#include <qmdnsengine/query.h>
#include <qmdnsengine/record.h>

namespace QMdnsEngine
{

const quint16 A = 1;
const quint16 AAAA = 28;
const quint16 NSEC = 47;
const quint16 PTR = 12;
const quint16 SRV = 33;
const quint16 TXT = 16;

template<class T>
bool parseInteger(const QByteArray &packet, quint16 &offset, T &value)
{
    if (offset + sizeof(T) > packet.length()) {
        return false;  // out-of-bounds
    }
    value = qFromBigEndian<T>(reinterpret_cast<const uchar*>(packet.constData() + offset));
    offset += sizeof(T);
    return true;
}

bool parseName(const QByteArray &packet, quint16 &offset, QByteArray &name)
{
    quint16 offsetEnd = 0;
    quint16 offsetPtr = offset;
    forever {
        quint8 nBytes;
        if (!parseInteger<quint8>(packet, offset, nBytes)) {
            return false;
        }
        if (!nBytes) {
            break;
        }
        switch (nBytes & 0xc0) {
        case 0x00:
            if (offset + nBytes > packet.length()) {
                return false;  // length exceeds message
            }
            name.append(packet.mid(offset, nBytes));
            name.append('.');
            offset += nBytes;
            break;
        case 0xc0:
        {
            quint8 nBytes2;
            quint16 newOffset;
            if (!parseInteger<quint8>(packet, offset, nBytes2)) {
                return false;
            }
            newOffset = ((nBytes & ~0xc0) << 8) | nBytes2;
            if (newOffset >= offsetPtr) {
                return false;  // prevent infinite loop
            }
            offsetPtr = newOffset;
            if (!offsetEnd) {
                offsetEnd = offset;
            }
            offset = newOffset;
            break;
        }
        default:
            return false;  // no other types supported
        }
    }
    if (offsetEnd) {
        offset = offsetEnd;
    }
    return true;
}

bool fromPacket(const QByteArray &packet, Message &message)
{
    quint16 offset = 0;
    quint16 transactionId, flags, nQuestion, nAnswer, nAuthority, nAdditional;
    if (!parseInteger<quint16>(packet, offset, transactionId) ||
            !parseInteger<quint16>(packet, offset, flags) ||
            !parseInteger<quint16>(packet, offset, nQuestion) ||
            !parseInteger<quint16>(packet, offset, nAnswer) ||
            !parseInteger<quint16>(packet, offset, nAuthority) ||
            !parseInteger<quint16>(packet, offset, nAdditional)) {
        return false;
    }
    message.setTransactionId(transactionId);
    message.setResponse(flags & 0x8400);
    for (int i = 0; i < nQuestion; ++i) {
        QByteArray name;
        quint16 type, class_;
        if (!parseName(packet, offset, name) ||
                !parseInteger<quint16>(packet, offset, type) ||
                !parseInteger<quint16>(packet, offset, class_)) {
            return false;
        }
        Query query;
        query.setName(name);
        query.setType(type);
        query.setUnicastResponse(class_ & 0x8000);
        message.addQuery(query);
    }
    quint16 nRecord = nAnswer + nAuthority + nAdditional;
    for (int i = 0; i < nRecord; ++i) {
        QByteArray name;
        quint16 type, class_, dataLen;
        quint32 ttl;
        if (!parseName(packet, offset, name) ||
                !parseInteger<quint16>(packet, offset, type) ||
                !parseInteger<quint16>(packet, offset, class_) ||
                !parseInteger<quint32>(packet, offset, ttl) ||
                !parseInteger<quint16>(packet, offset, dataLen)) {
            return false;
        }
        Record record;
        record.setName(name);
        record.setType(type);
        record.setFlushCache(class_ & 0x8000);
        record.setTtl(ttl);
        switch (type) {
        case A:
        {
            quint32 ipv4Addr;
            if (!parseInteger<quint32>(packet, offset, ipv4Addr)) {
                return false;
            }
            record.setAddress(QHostAddress(ipv4Addr));
            break;
        }
        case AAAA:
        {
            if (offset + 16 > packet.length()) {
                return false;
            }
            record.setAddress(QHostAddress(
                reinterpret_cast<const quint8*>(packet.constData() + offset)
            ));
            offset += 16;
            break;
        }
        case NSEC:
        {
            QByteArray nextDomainName;
            quint8 number;
            quint8 length;
            if (!parseName(packet, offset, nextDomainName) ||
                    !parseInteger<quint8>(packet, offset, number) ||
                    !parseInteger<quint8>(packet, offset, length) ||
                    number != 0 ||
                    offset + length > packet.length()) {
                return false;
            }
            Bitmap bitmap;
            bitmap.setData(length, reinterpret_cast<const quint8*>(packet.constData() + offset));
            record.setNextDomainName(nextDomainName);
            record.setBitmap(bitmap);
            break;
        }
        case PTR:
        {
            QByteArray target;
            if (!parseName(packet, offset, target)) {
                return false;
            }
            record.setTarget(target);
            break;
        }
        case SRV:
        {
            quint16 priority, weight, port;
            QByteArray target;
            if (!parseInteger<quint16>(packet, offset, priority) ||
                    !parseInteger<quint16>(packet, offset, weight) ||
                    !parseInteger<quint16>(packet, offset, port) ||
                    !parseName(packet, offset, target)) {
                return false;
            }
            record.setPriority(priority);
            record.setWeight(weight);
            record.setPort(port);
            record.setTarget(target);
            break;
        }
        case TXT:
        {
            quint16 start = offset;
            while (offset < start + dataLen) {
                quint8 nBytes;
                if (!parseInteger<quint8>(packet, offset, nBytes) ||
                        offset + nBytes > packet.length()) {
                    return false;
                }
                QByteArray attr(packet.constData() + offset, nBytes);
                offset += nBytes;
                int splitIndex = attr.indexOf('=');
                if (splitIndex == -1) {
                    return false;
                }
                record.addAttribute(attr.left(splitIndex), attr.mid(splitIndex + 1));
            }
            break;
        }
        default:
            offset += dataLen;
            break;
        }
        message.addRecord(record);
    }
    return true;
}

}
