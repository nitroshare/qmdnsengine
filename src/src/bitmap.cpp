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

#include <qmdnsengine/bitmap.h>

#include "bitmap_p.h"

using namespace QMdnsEngine;

BitmapPrivate::BitmapPrivate()
    : length(0)
{
}

Bitmap::Bitmap()
    : d(new BitmapPrivate)
{
}

Bitmap::Bitmap(const Bitmap &other)
    : d(new BitmapPrivate)
{
    *this = other;
}

Bitmap &Bitmap::operator=(const Bitmap &other)
{
    *d = *other.d;
    return *this;
}

Bitmap::~Bitmap()
{
    delete d;
}

quint8 Bitmap::length() const
{
    return d->length;
}

const quint8 *Bitmap::data() const
{
    return d->data;
}

void Bitmap::setData(quint8 length, const quint8 *data)
{
    d->length = length;
    for (int i = 0; i < d->length; ++i) {
        d->data[i] = data[i];
    }
}
