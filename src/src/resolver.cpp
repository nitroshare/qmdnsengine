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
#include <qmdnsengine/record.h>
#include <qmdnsengine/resolver.h>
#include <qmdnsengine/server.h>

#include "resolver_p.h"

using namespace QMdnsEngine;

ResolverPrivate::ResolverPrivate(Resolver *resolver, Server *server, const QByteArray &name)
    : QObject(resolver),
      q(resolver),
      server(server),
      name(name)
{
    connect(server, &Server::messageReceived, this, &ResolverPrivate::onMessageReceived);

    query();
}

void ResolverPrivate::query()
{
    Query query;
    query.setName(name);
    query.setType(A);
    Message message;
    message.addQuery(query);
    query.setType(AAAA);
    message.addQuery(query);
    server->broadcastMessage(message);
}

void ResolverPrivate::onMessageReceived(const Message &message)
{
    if (!message.isResponse()) {
        return;
    }
    foreach (Record record, message.records()) {
        if (record.name() == name && (record.type() == A || record.type() == AAAA)) {
            emit q->resolved(record.address());
        }
    }
}

Resolver::Resolver(Server *server, const QByteArray &name, QObject *parent)
    : QObject(parent),
      d(new ResolverPrivate(this, server, name))
{
}
