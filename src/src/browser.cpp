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

#include <QSet>

#include <qmdnsengine/browser.h>
#include <qmdnsengine/cache.h>
#include <qmdnsengine/dns.h>
#include <qmdnsengine/message.h>
#include <qmdnsengine/query.h>
#include <qmdnsengine/record.h>
#include <qmdnsengine/server.h>

#include "browser_p.h"

using namespace QMdnsEngine;

BrowserPrivate::BrowserPrivate(Browser *browser, Server *server, const QByteArray &type, Cache *existingCache)
    : QObject(browser),
      q(browser),
      server(server),
      type(type),
      cache(existingCache)
{
    if (!cache) {
        cache = new Cache;
    }

    connect(server, &Server::messageReceived, this, &BrowserPrivate::onMessageReceived);

    connect(cache, &Cache::shouldQuery, this, &BrowserPrivate::onShouldQuery);
    connect(cache, &Cache::recordExpired, this, &BrowserPrivate::onRecordExpired);
    connect(&timer, &QTimer::timeout, this, &BrowserPrivate::onTimeout);

    timer.setSingleShot(true);
    onTimeout();
}

// TODO: multiple SRV records not supported

void BrowserPrivate::updateService(const QByteArray &name)
{
    // Determine if a service entry already exists in the map
    Service oldService = services.value(name);

    // Create the service if the PTR and SRV record are present; add the
    // TXT attributes if they exist
    Record ptrRecord, srvRecord;
    QByteArray serviceType = name.mid(name.indexOf('.') + 1);
    if (cache->lookupRecord(serviceType, PTR, ptrRecord) &&
            cache->lookupRecord(name, SRV, srvRecord)) {
        Service service;
        service.setName(name);
        service.setType(type);
        service.setPort(srvRecord.port());

        // If TXT records are available for the service, add their values
        QList<Record> txtRecords;
        if (cache->lookupRecords(name, TXT, txtRecords)) {
            QMap<QByteArray, QByteArray> attributes;
            foreach (Record record, txtRecords) {
                for (auto i = record.attributes().constBegin();
                        i != record.attributes().constEnd(); ++i) {
                    attributes.insert(i.key(), i.value());
                }
            }
            service.setAttributes(attributes);
        }

        // If the service existed, this is an update; otherwise it is a
        // new addition; emit the appropriate signal
        if (oldService.name().isNull()) {
            emit q->serviceAdded(service);
        } else {
            emit q->serviceUpdated(service);
        }
        services.insert(name, service);

    } else {

        // If the PTR or SRV record are missing (and the service existed
        // before), then it is no longer available - remove it
        if (!oldService.name().isNull()) {
            emit q->serviceRemoved(oldService);
            services.remove(name);
        }
    }
}

void BrowserPrivate::onMessageReceived(const Message &message)
{
    if (!message.isResponse()) {
        return;
    }

    // Use a set to track all services that are updated in the message - this
    // avoids extraneous signals being emitted if SRV and TXT are provided
    QSet<QByteArray> serviceNames;
    foreach (Record record, message.records()) {
        if (record.type() == PTR && record.name() == type ||
                record.type() == SRV && record.name().endsWith("." + type) ||
                record.type() == TXT && record.name().endsWith("." + type)) {
            cache->addRecord(record);
            switch (record.type()) {
            case PTR:
                serviceNames.insert(record.target());
                break;
            case SRV:
            case TXT:
                serviceNames.insert(record.name());
                break;
            }
        }
    }
    foreach (QByteArray name, serviceNames) {
        updateService(name);
    }
}

void BrowserPrivate::onShouldQuery(const Record &record)
{
    // Assume that all messages in the cache are still in use (by the browser)
    // and attempt to renew them immediately

    Query query;
    query.setName(record.name());
    query.setType(record.type());
    Message message;
    message.addQuery(query);
    server->broadcastMessage(message);
}

void BrowserPrivate::onRecordExpired(const Record &record)
{
    switch (record.type()) {
    case PTR:
        updateService(record.target());
        break;
    case SRV:
    case TXT:
        updateService(record.name());
        break;
    }
}

void BrowserPrivate::onTimeout()
{
    Query query;
    query.setName(type);
    query.setType(PTR);
    Message message;
    message.addQuery(query);

    // TODO: including too many records could cause problems

    // Include all currently valid PTR records
    QList<Record> records;
    if (cache->lookupRecords(QByteArray(), PTR, records)) {
        foreach (Record record, records) {
            message.addRecord(record);
        }
    }

    server->broadcastMessage(message);
    timer.start(60 * 1000);
}

Browser::Browser(Server *server, const QByteArray &type, Cache *cache, QObject *parent)
    : QObject(parent),
      d(new BrowserPrivate(this, server, type, cache))
{
}
