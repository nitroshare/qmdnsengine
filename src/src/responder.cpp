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

// TODO: this loop is terribly inefficient
// TODO: in theory, this loop could cause a record to be returned multiple times

void ResponderPrivate::onMessageReceived(const Message &message)
{
    if (message.isResponse()) {
        return;
    }
    Message reply;
    reply.reply(message);
    foreach (Query query, message.queries()) {
        for (auto i = records.constBegin(); i != records.constEnd(); ++i) {
            if (query.name() == i->name() && (query.type() == ANY || query.type() == i->type())) {
                reply.addRecord(*i);
            }
        }
    }
    if (reply.records().count()) {
        server->sendMessage(message);
    }
}

Responder::Responder(Server *server, QObject *parent)
    : QObject(parent),
      d(new ResponderPrivate(parent, server))
{
}

void Responder::addRecord(const Record &record)
{
    d->records.append(record);
}
