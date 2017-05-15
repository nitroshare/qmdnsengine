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
#include <QHostInfo>
#include <QNetworkAddressEntry>
#include <QNetworkInterface>

#include <qmdnsengine/dns.h>
#include <qmdnsengine/hostname.h>
#include <qmdnsengine/message.h>
#include <qmdnsengine/query.h>
#include <qmdnsengine/record.h>
#include <qmdnsengine/server.h>

#include "hostname_p.h"

using namespace QMdnsEngine;

HostnamePrivate::HostnamePrivate(Hostname *hostname, Server *server)
    : QObject(hostname),
      q(hostname),
      server(server)
{
    connect(server, &Server::messageReceived, this, &HostnamePrivate::onMessageReceived);
    connect(&timer, &QTimer::timeout, this, &HostnamePrivate::onTimeout);

    timer.setSingleShot(true);

    // Immediately begin asserting the hostname
    assertHostname();
}

void HostnamePrivate::assertHostname()
{
    hostnameRegistered = false;
    hostnameSuffix = 1;

    broadcastHostname();
}

void HostnamePrivate::broadcastHostname()
{
    QByteArray localHostname = QHostInfo::localHostName().toUtf8();
    hostname = hostnameSuffix == 1 ? localHostname:
        localHostname + "-" + QByteArray::number(hostnameSuffix);

    Query ipv4Query;
    ipv4Query.setName(hostname);
    ipv4Query.setType(A);
    Query ipv6Query;
    ipv6Query.setName(hostname);
    ipv6Query.setType(AAAA);
    Message message;
    message.addQuery(ipv4Query);
    message.addQuery(ipv6Query);

    server->broadcastMessage(message);

    // If no reply is received after two seconds, the hostname is available
    timer.stop();
    timer.start(2 * 1000);
}

bool HostnamePrivate::generateRecord(const QHostAddress &srcAddress, quint16 type, Record &record)
{
    // Attempt to find the interface that corresponds with the provided
    // address and determine this device's address from the interface.

    foreach (QNetworkInterface interface, QNetworkInterface::allInterfaces()) {
        foreach (QNetworkAddressEntry entry, interface.addressEntries()) {
            if (srcAddress.isInSubnet(entry.ip(), entry.prefixLength())) {
                foreach (QHostAddress address, interface.allAddresses()) {
                    if ((address.protocol() == QAbstractSocket::IPv4Protocol && type == A) ||
                            (address.protocol() == QAbstractSocket::IPv6Protocol && type == AAAA)) {
                        record.setName(hostname);
                        record.setType(type);
                        record.setAddress(address);
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void HostnamePrivate::onMessageReceived(const Message &message)
{
    if (message.isResponse()) {
        foreach (Record record, message.records()) {
            if ((record.type() == A || record.type() == AAAA) && record.name() == hostname) {
                ++hostnameSuffix;
                broadcastHostname();
            }
        }
    } else {
        Message reply;
        reply.reply(message);
        foreach (Query query, message.queries()) {
            if (hostnameRegistered && (query.type() == A || query.type() == AAAA) &&
                    query.name() == hostname) {
                Record record;
                if (generateRecord(message.address(), query.type(), record)) {
                    reply.addRecord(record);
                }
            }
        }
        if (reply.records().count()) {
            server->sendMessage(message);
        }
    }
}

void HostnamePrivate::onTimeout()
{
    hostnameRegistered = true;
    emit q->hostnameChanged(hostname);
}

Hostname::Hostname(Server *server, QObject *parent)
    : QObject(parent),
      d(new HostnamePrivate(this, server))
{
}

bool Hostname::isRegistered() const
{
    return d->hostnameRegistered;
}

QByteArray Hostname::hostname() const
{
    return d->hostname;
}
