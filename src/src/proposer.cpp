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

#include <qmdnsengine/message.h>
#include <qmdnsengine/proposer.h>
#include <qmdnsengine/query.h>
#include <qmdnsengine/server.h>

#include "proposer_p.h"

using namespace QMdnsEngine;

ProposerPrivate::ProposerPrivate(Proposer *proposer, Server *server, const Record &record)
    : QObject(proposer),
      q(proposer),
      server(server),
      confirmed(false),
      proposedRecord(record),
      suffix(1)
{
    // All records should contain at least one "."
    int index = record.name().indexOf('.');
    name = record.name().left(index);
    type = record.name().mid(index);

    connect(server, &Server::messageReceived, this, &ProposerPrivate::onMessageReceived);
    connect(&timer, &QTimer::timeout, this, &ProposerPrivate::onTimeout);

    timer.setSingleShot(true);
}

void ProposerPrivate::assertHostname()
{
    // Use the current suffix to set the name of the proposed record
    if (suffix == 1) {
        proposedRecord.setName(name + type);
    } else {
        proposedRecord.setName(name + "-" + QByteArray::number(suffix) + type);
    }

    // Broadcast a query containing the proposed record
    Query query;
    query.setName(proposedRecord.name());
    query.setType(proposedRecord.type());
    Message message;
    message.addQuery(query);
    message.addRecord(proposedRecord);
    server->broadcastMessage(message);

    // Wait two seconds to confirm it is unique
    timer.stop();
    timer.start(2 * 1000);
}

void ProposerPrivate::onMessageReceived(const Message &message)
{
    // If the response matches the proposed record, increment the suffix and
    // try with the new name

    if (confirmed || !message.isResponse()) {
        return;
    }
    foreach (Record record, message.records()) {
        if (record.name() == proposedRecord.name() && record.type() == proposedRecord.type()) {
            ++suffix;
            assertHostname();
        }
    }
}

void ProposerPrivate::onTimeout()
{
    confirmed = true;
    emit q->recordConfirmed(proposedRecord);
}

Proposer::Proposer(Server *server, const Record &record, QObject *parent)
    : QObject(parent),
      d(new ProposerPrivate(this, server, record))
{
}
