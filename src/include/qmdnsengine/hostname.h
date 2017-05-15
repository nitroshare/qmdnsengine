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

#ifndef QMDNSENGINE_HOSTNAME_H
#define QMDNSENGINE_HOSTNAME_H

#include <QObject>

#include "qmdnsengine_export.h"

namespace QMdnsEngine
{

class Server;

class QMDNSENGINE_EXPORT HostnamePrivate;

/**
 * @brief Register and utilize a unique hostname
 *
 * In order to use SRV records, a unique hostname must be used. This class
 * asserts a hostname (by first confirming that it is not in use) and then
 * responds to A and AAAA queries for the hostname.
 */
class QMDNSENGINE_EXPORT Hostname : public QObject
{
    Q_OBJECT

public:

    Hostname(Server *server, QObject *parent = 0);

    /**
     * @brief Determines if a hostname has been registered
     */
    bool isRegistered() const;

    /**
     * @brief Retrieve the current hostname
     *
     * This value is only valid when isRegistered() returns true.
     */
    QByteArray hostname() const;

Q_SIGNALS:

    /**
     * @brief Indicate that the current hostname has changed
     */
    void hostnameChanged(const QByteArray &hostname);

private:

    HostnamePrivate *const d;
};

}

#endif // QMDNSENGINE_HOSTNAME_H
