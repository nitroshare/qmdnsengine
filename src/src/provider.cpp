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
#include <qmdnsengine/provider.h>
#include <qmdnsengine/server.h>

#include "provider_p.h"

using namespace QMdnsEngine;

ProviderPrivate::ProviderPrivate(QObject *parent, Server *server, Hostname *hostname)
    : QObject(parent),
      server(server),
      hostname(hostname),
      responder(server),
      initialized(false)
{
    connect(hostname, &Hostname::hostnameChanged, this, &ProviderPrivate::onHostnameChanged);

    browsePtrRecord.setType(PTR);
    ptrRecord.setType(PTR);
    srvRecord.setType(SRV);
    txtRecord.setType(TXT);
}

void ProviderPrivate::updateRecords()
{
    if (!hostname->isRegistered()) {
        return;
    }

    if (initialized) {
        responder.removeRecord(browsePtrRecord);
        responder.removeRecord(ptrRecord);
        responder.removeRecord(srvRecord);
        responder.removeRecord(txtRecord);
    }

    QByteArray fqName = service.name() + "." + service.type();

    browsePtrRecord.setName(MdnsBrowseType);
    browsePtrRecord.setTarget(service.type());

    ptrRecord.setName(service.type());
    ptrRecord.setTarget(fqName);

    srvRecord.setName(fqName);
    srvRecord.setPort(service.port());
    srvRecord.setTarget(hostname->hostname());

    txtRecord.setName(fqName);
    txtRecord.setAttributes(service.attributes());

    responder.addRecord(browsePtrRecord, false);
    responder.addRecord(ptrRecord, false);
    responder.addRecord(srvRecord, true);
    responder.addRecord(txtRecord, true);

    initialized = true;
}

void ProviderPrivate::onHostnameChanged(const QByteArray &hostname)
{
    updateRecords();
}

Provider::Provider(Server *server, Hostname *hostname, QObject *parent)
    : QObject(parent),
      d(new ProviderPrivate(this, server, hostname))
{
}

void Provider::update(const Service &service)
{
    d->service = service;
    d->updateRecords();
}
