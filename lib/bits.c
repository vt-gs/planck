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

/* Common bit packaging routines */
// packed into a byte from left to right
// array [0, 0, 0, 0,  1, 1, 1, 1] -> bin 00001111 -> 0x0f
unsigned char pk_pack_byte_lr(const unsigned char *bits)
{
    unsigned char result = 0;

    size_t i;
    for (i = 0; i < 8; i++)
        result |= (bits[7 - i] & 1) << i;

    return result;
}

// unpacked from left to right
// 0x0f -> bin 00001111 -> array [0, 0, 0, 0,  1, 1, 1, 1]
void pk_unpack_byte_lr(unsigned char *bits, unsigned char byte)
{
    size_t i;
    for (i = 0; i < 8; i++) {
        bits[i] = (byte << i) & 0x80;
        bits[i] = (bits[i] >> 7) & 1;
    }
}

// packed into a byte from right to left
// array [0, 0, 0, 0,  1, 1, 1, 1] -> bin 11110000 -> 0xf0
unsigned char pk_pack_byte_rl(const unsigned char *bits)
{
    unsigned char result = 0;

    size_t i;
    for (i = 0; i < 8; i++)
        result |= (bits[i] & 1) << i;

    return result;
}

// unpacked from left to right
// 0xf0 -> bin 11110000 -> array [0, 0, 0, 0,  1, 1, 1, 1]
void pk_unpack_byte_rl(unsigned char *bits, unsigned char byte)
{
    size_t i;
    for (i = 0; i < 8; i++) {
        bits[i] = (byte << (7 - i)) & 0x80;
        bits[i] = (bits[i] >> 7) & 1;
    }
}
