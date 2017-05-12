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

#ifndef QMDNSENGINE_BROWSER_H
#define QMDNSENGINE_BROWSER_H

#include <QByteArray>
#include <QObject>

#include "qmdnsengine_export.h"

namespace QMdnsEngine
{

class Cache;
class Server;
class Service;

class QMDNSENGINE_EXPORT BrowserPrivate;

/**
 * @brief Find local services
 */
class QMDNSENGINE_EXPORT Browser : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Create a new browser instance
     * @param server server to use for receiving and sending mDNS messages
     * @param type service type to browse for
     * @param cache DNS cache to use or null to create one
     * @param parent QObject
     */
    Browser(Server *server, const QByteArray &type, Cache *cache = 0, QObject *parent = 0);

Q_SIGNALS:

    /**
     * @brief Indicate that a new service has been added
     */
    void serviceAdded(const Service &service);

    /**
     * @brief Indicate that the specified service was updated
     *
     * This signal is emitted when the SRV record for a service (identified by
     * its name and type) has changed.
     */
    void serviceUpdated(const Service &service);

    /**
     * @brief Indicate that the specified service was removed
     */
    void serviceRemoved(const Service &service);

private:

    BrowserPrivate *const d;
};

}

#endif // QMDNSENGINE_BROWSER_H
