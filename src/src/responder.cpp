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
#include <qmdnsengine/message.h>
#include <qmdnsengine/prober.h>
#include <qmdnsengine/query.h>
#include <qmdnsengine/responder.h>
#include <qmdnsengine/server.h>

#include "responder_p.h"

using namespace QMdnsEngine;

ResponderPrivate::ResponderPrivate(QObject *parent, Server *server)
    : QObject(parent),
      server(server)
{
    connect(server, &Server::messageReceived, this, &ResponderPrivate::onMessageReceived);
}

void ResponderPrivate::onMessageReceived(const Message &message)
{
    if (message.isResponse()) {
        return;
    }
    Message reply;
    reply.reply(message);
    foreach (Query query, message.queries()) {
        foreach (Record record, records.values(query.name())) {
            if (query.type() == record.type()) {
                reply.addRecord(record);
            }
        }
    }
    if (reply.records().count()) {
        server->sendMessage(message);
    }
}

void ResponderPrivate::translate(Record &record) const
{
    // Take the record and check to see if its name should be modified due to
    // another (existing) record on the network

    record.setName(renames.value(record.name(), record.name()));
}

void ResponderPrivate::insertRecords(const QByteArray &oldName, const QByteArray &newName)
{
    foreach (Record record, pendingRecords.values(oldName)) {
        record.setName(newName);
        records.insert(newName, record);
    }
    pendingRecords.remove(oldName);
    renames.insert(oldName, newName);
}

Responder::Responder(Server *server, QObject *parent)
    : QObject(parent),
      d(new ResponderPrivate(parent, server))
{
}

void Responder::addRecord(const Record &record)
{
    Record trRecord = record;
    d->translate(trRecord);

    if (d->records.count(trRecord.name())) {

        // If a record with the same name exists in records, then it is safe to
        // directly insert this new one
        d->records.insert(trRecord.name(), trRecord);

    } else {

        // Either a pending probe exists (in which case, this record can be
        // added to the pending list) or one does not (in which case, a new
        // probe is started)
        d->pendingRecords.insert(record.name(), record);
        if (!d->pendingRecords.count(record.name())) {
            Prober *prober = new Prober(d->server, record, this);
            connect(prober, &Prober::recordConfirmed, [this, record](const Record &newRecord) {
                d->insertRecords(record.name(), newRecord.name());
            });
        }
    }
}
