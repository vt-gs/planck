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

/* Filter design functions */
void pk_firdes_gaussian(
    float *taps,
    float bt,
    float delay,
    unsigned int span)
{
    unsigned int len = 2*span + 1;

    int i;
    for (i = 0; i < len; i++) {
        float t = (float) i / (float) span;

        taps[i]  = pk_qfunc(2*M_PI*bt*(t - 0.5f));
        taps[i] -= pk_qfunc(2*M_PI*bt*(t + 0.5f));
        taps[i] *= (1.0f / M_LN2);
    }
}

void pk_firdes_gaussian_noisi(
    float *taps,
    float bt,
    float delay,
    unsigned int span)
{
    // TODO: Implement a filter design to reduce ISI
    // due to the Gaussian TX filter
}

void pk_firdes_rrc(
    float *taps,
    float beta,
    float delay,
    unsigned int span)
{
    // TODO: Implement the root-raised cosine pulse
}

void pk_firdes_srrc(
    float *taps,
    float beta,
    float delay,
    unsigned int span)
{
    // TODO: Implement the square root-raised cosine pulse
}
