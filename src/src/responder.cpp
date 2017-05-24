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

QByteArray ResponderPrivate::translate(const QByteArray &name) const
{
    return renames.value(name, name);
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
    QByteArray name = d->translate(record.name());
    if (d->records.count(name)) {

        // If a record with the same name exists in records, then it is safe to
        // insert the record (changing its name to match)
        Record newRecord = record;
        newRecord.setName(name);
        d->records.insert(name, newRecord);

    } else {

        // Either a pending probe exists (in which case, this record can be
        // added to the pending list) or one does not (in which case, a new
        // probe is started)
        if (!d->pendingRecords.count(name)) {
            Prober *prober = new Prober(d->server, record, this);
            connect(prober, &Prober::nameConfirmed, [this, name](const QByteArray &newName) {
                d->insertRecords(name, newName);
            });
        }
        d->pendingRecords.insert(name, record);
    }
}

void Responder::removeRecord(const Record &record)
{
    // Remove all active records that match
    QByteArray name = d->translate(record.name());
    for (auto i = d->records.find(name); i != d->records.end() && i.key() == name;) {
        if (i.value() == record) {
            i = d->records.erase(i);
        } else {
            ++i;
        }
    }

    // Remove the entry from the translation map if no records are using it
    if (!d->records.count(name)) {
        d->renames.remove(record.name());
    }

    // Remove pending records that match
    d->pendingRecords.remove(record.name(), record);
}
