/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Paul Uri David
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "plancki.h"

/* CRC and error detection routines */
// Algorithm comes from Kenneth W. Finnegan's APRS thesis.
// This particular CRC function is appropriate for AX.25
unsigned int crc_ax25_byte(const unsigned char *data, size_t size)
{
    unsigned int crc = 0xffff;
    unsigned int mask = 0;

    unsigned char byte = 0;
    unsigned char bit  = 0;

    size_t i, j;
    for (i = 0; i < size; i++) {
        byte = data[i];
        for (j = 0; j < 8; j++) {
            bit = (byte >> j) & 1;
            mask = 0x8408 & -((crc & 1) ^ bit);
            crc = (crc >> 1) ^ mask;
        }
    }

    return crc;
}
