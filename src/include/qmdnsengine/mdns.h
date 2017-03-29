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

#ifndef QMDNSENGINE_MDNS_H
#define QMDNSENGINE_MDNS_H

#include <QHostAddress>

#include "qmdnsengine_export.h"

namespace QMdnsEngine
{

/**
 * @brief DNS record types
 */
enum {
    A = 1,
    AAAA = 28,
    NSEC = 47,
    PTR = 12,
    SRV = 33,
    TXT = 16
};

/**
 * @brief Retrieve the standard port for mDNS
 */
QMDNSENGINE_EXPORT quint16 mdnsPort();

/**
 * @brief Retrieve the IPv4 address to send mDNS queries to
 */
QMDNSENGINE_EXPORT QHostAddress mdnsIpv4Address();

/**
 * @brief Retrieve the IPv6 address to send mDNS queries to
 */
QMDNSENGINE_EXPORT QHostAddress mdnsIpv6Address();

}

#endif // QMDNSENGINE_MDNS_H
