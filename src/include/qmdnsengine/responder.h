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

#ifndef QMDNSENGINE_RESPONDER_H
#define QMDNSENGINE_RESPONDER_H

#include <QObject>

#include "qmdnsengine_export.h"

namespace QMdnsEngine
{

class Record;
class Server;

class QMDNSENGINE_EXPORT ResponderPrivate;

/**
 * @brief Respond to DNS queries for records
 */
class QMDNSENGINE_EXPORT Responder : public QObject
{
    Q_OBJECT

public:

    Responder(Server *server, QObject *parent = 0);

    /**
     * @brief Add a record to the responder
     * @param record DNS record to add
     * @param probe true to probe for the record
     *
     * The responder keeps track of which names it has exclusive use of and if
     * it does not have exclusive use of the name, it will send a DNS ANY
     * to confirm that it is safe to use. If the name is in use, all records
     * added via this method will have their names changed.
     */
    void addRecord(const Record &record, bool probe);

    /**
     * @brief Remove a record from the responder
     * @param record DNS record to remove
     */
    void removeRecord(const Record &record);

private:

    ResponderPrivate *const d;
};

}

#endif // QMDNSENGINE_RESPONDER_H
