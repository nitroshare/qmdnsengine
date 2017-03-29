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

#ifndef QMDNSENGINE_BITMAP_H
#define QMDNSENGINE_BITMAP_H

#include "qmdnsengine_export.h"

namespace QMdnsEngine
{

class QMDNSENGINE_EXPORT BitmapPrivate;

/**
 * @brief 256-bit bitmap
 *
 * Bitmaps are used in QMdnsEngine::NSEC records to indicate which records are
 * available. mDNS only uses the first block (0).
 */
class QMDNSENGINE_EXPORT Bitmap
{
public:

    Bitmap();
    Bitmap(const Bitmap &other);
    Bitmap &operator=(const Bitmap &other);
    virtual ~Bitmap();

    /**
     * @brief Retrieve the length of the block in bytes
     */
    quint8 length() const;

    /**
     * @brief Retrieve a pointer to the underlying data in the bitmap
     *
     * Use the length() method to determine how many bytes contain valid data.
     */
    const quint8 *data() const;

    /**
     * @brief Set the data to be stored in the bitmap
     */
    void setData(quint8 length, const quint8 *data);

private:

    BitmapPrivate *const d;
};

}

#endif // QMDNSENGINE_BITMAP_H
