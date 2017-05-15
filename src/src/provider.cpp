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

#include <qmdnsengine/dns.h>
#include <qmdnsengine/hostname.h>
#include <qmdnsengine/mdns.h>
#include <qmdnsengine/message.h>
#include <qmdnsengine/provider.h>
#include <qmdnsengine/query.h>
#include <qmdnsengine/server.h>
#include <qmdnsengine/service.h>

#include "provider_p.h"

using namespace QMdnsEngine;

const quint32 DefaultTtl = 3600;

ProviderPrivate::ProviderPrivate(QObject *parent, Server *server, Hostname *hostname)
    : QObject(parent),
      server(server),
      hostname(hostname)
{
    browsePtrRecord.setType(PTR);
    ptrRecord.setType(PTR);
    srvRecord.setType(SRV);
    txtRecord.setType(TXT);

    connect(server, &Server::messageReceived, this, &ProviderPrivate::onMessageReceived);
    connect(hostname, &Hostname::hostnameChanged, this, &ProviderPrivate::onHostnameChanged);
}

void ProviderPrivate::updateRecords(const Service &service)
{
    browsePtrRecord.setName(MdnsBrowseType);
    browsePtrRecord.setTarget(service.type());

    ptrRecord.setName(service.type());
    ptrRecord.setTarget(service.name());

    srvRecord.setName(service.name());
    srvRecord.setPort(service.port());

    txtRecord.setName(service.name());
    txtRecord.setAttributes(service.attributes());
}

void ProviderPrivate::onMessageReceived(const Message &message)
{
    if (!hostname->isRegistered() || message.isResponse()) {
        return;
    }
    Message reply;
    reply.reply(message);
    foreach (Query query, message.queries()) {
        if (query.type() == PTR && query.name() == browsePtrRecord.name()) {
            reply.addRecord(browsePtrRecord);
        } else if (query.type() == PTR && query.name() == ptrRecord.name()) {
            reply.addRecord(ptrRecord);
        } else if (query.type() == SRV && query.name() == srvRecord.name()) {
            reply.addRecord(srvRecord);
        } else if (query.type() == TXT && query.name() == txtRecord.name()) {
            reply.addRecord(txtRecord);
        }
    }
    if (message.records().count()) {
        server->sendMessage(message);
    }
}

void ProviderPrivate::onHostnameChanged(const QByteArray &hostname)
{
    srvRecord.setTarget(hostname);
}

Provider::Provider(Server *server, Hostname *hostname, QObject *parent)
    : QObject(parent),
      d(new ProviderPrivate(this, server, hostname))
{
}

void Provider::update(const Service &service)
{
    d->updateRecords(service);
}
