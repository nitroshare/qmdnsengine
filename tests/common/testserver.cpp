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

#include <qmdnsengine/mdns.h>

#include "testserver.h"

void TestServer::sendMessage(const QMdnsEngine::Message &message)
{
    mMessages.append(message);
}

void TestServer::sendMessageToAll(const QMdnsEngine::Message &message)
{
    QMdnsEngine::Message ipv4Message = message;
    ipv4Message.setAddress(QMdnsEngine::MdnsIpv4Address);
    ipv4Message.setPort(QMdnsEngine::MdnsPort);
    mMessages.append(ipv4Message);
    QMdnsEngine::Message ipv6Message = message;
    ipv4Message.setAddress(QMdnsEngine::MdnsIpv6Address);
    ipv4Message.setPort(QMdnsEngine::MdnsPort);
    mMessages.append(ipv6Message);
}

void TestServer::deliverMessage(const QMdnsEngine::Message &message)
{
    emit messageReceived(message);
}

QList<QMdnsEngine::Message> TestServer::receivedMessages() const
{
    return mMessages;
}
