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

#ifndef QMDNSENGINE_DNS_H
#define QMDNSENGINE_DNS_H

#include <QByteArray>

#include "qmdnsengine_export.h"

namespace QMdnsEngine
{

class Message;

QMDNSENGINE_EXPORT extern const quint16 A;
QMDNSENGINE_EXPORT extern const quint16 AAAA;
QMDNSENGINE_EXPORT extern const quint16 NSEC;
QMDNSENGINE_EXPORT extern const quint16 PTR;
QMDNSENGINE_EXPORT extern const quint16 SRV;
QMDNSENGINE_EXPORT extern const quint16 TXT;

/**
 * @brief Parse a name from a raw DNS packet
 * @param packet raw DNS packet data
 * @param offset offset into the packet where the name begins
 * @param name reference to QByteArray to store the name in
 * @return true if no errors occurred
 */
QMDNSENGINE_EXPORT bool parseName(const QByteArray &packet, quint16 &offset, QByteArray &name);

/**
 * @brief Populate a Message with data from a raw DNS packet
 * @param packet raw DNS packet data
 * @param message reference to Message to populate
 * @return true if no errors occurred
 */
QMDNSENGINE_EXPORT bool fromPacket(const QByteArray &packet, Message &message);

}

#endif // QMDNSENGINE_DNS_H
