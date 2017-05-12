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

#ifndef QMDNSENGINE_SERVER_H
#define QMDNSENGINE_SERVER_H

#include <QObject>

#include "qmdnsengine_export.h"

namespace QMdnsEngine
{

class Message;

class QMDNSENGINE_EXPORT ServerPrivate;

/**
 * @brief mDNS server
 */
class QMDNSENGINE_EXPORT Server : public QObject
{
    Q_OBJECT

public:

    explicit Server(QObject *parent = 0);

    /**
     * @brief Send a message
     *
     * The server will use the message's address to determine which protocol
     * to use for sending it (IPv4 or IPv6).
     */
    void sendMessage(const Message &message);

    /**
     * @brief Broadcast a message
     *
     * Send a message to both the IPv4 and IPv6 addresses and port for mDNS.
     */
    void broadcastMessage(const Message &message);

Q_SIGNALS:

    /**
     * @brief Indicate a new message was received
     *
     * This signal is only emitted after successfully confirming a hostname.
     */
    void messageReceived(const Message &message);

    /**
     * @brief Indicate an error has occurred
     */
    void error(const QString &message);

private:

    ServerPrivate *const d;
};

}

#endif // QMDNSENGINE_SERVER_H
