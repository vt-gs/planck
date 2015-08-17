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

#ifndef INCLUDED_PLANCK_INTERNAL_H
#define INCLUDED_PLANCK_INTERNAL_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <complex.h>
#include <assert.h>

#include <planck.h>

// Preprocessor debug settings
#define VERBOSE   2
#define INFO      1
#define DISABLED  0
#define DEBUG  DISABLED

// AX.25 framing definitions
#define AX25_FLAG       0x7e
#define AX25_STUFFED    0x3e

#define MAX_AX25_BYTES  412
#define MIN_AX25_BYTES  17
#define MAX_AX25_ADDR   70

#define AX25_FCS_BYTES  2
#define AX25_CRC_MAGIC  0xf0b8

// maximal polynomial table
static uint32_t lfsr_poly_tab[] = {
    0x000e4001, 0x00040801, 0x00021001, 0x0001a011, // 19, 18, 17, 16
    0x0000c001, 0x00004029, 0x00002019, 0x00001051, // 15, 14, 13, 12
    0x00000a01, 0x00000481, 0x00000221, 0x00000171, // 11, 10,  9,  8
    0x000000c1, 0x00000061, 0x00000029, 0x00000019, //  7,  6,  5,  4
    0x0000000d, 0x00000007,                         //  3,  2
};

/* common math functions */
// L2 distance for float complex
float pk_dist_cf(float complex a, float complex b);

// next power of 2
unsigned int pk_next2pow(unsigned int num);

// next raised power of 2
unsigned int pk_next2pow2(unsigned int num);

#endif
