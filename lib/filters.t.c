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

/* Simplified FIR filter structure */
typedef struct pk_fir_XX_s
{
    unsigned int order;

    // deal with the circular buffer
    // internally to avoid copies
    <I> *buffer;
    <O> *coeff;
    unsigned int diff;
    unsigned int mask;
    unsigned int index;
} pk_fir_XX;

pk_fir_XX *pk_fir_XX_create(unsigned int order, const <O> *coeff)
{
    pk_fir_XX *fir = malloc(sizeof(pk_fir_XX));
    fir->order = order;
    fir->diff = pk_next2pow2(fir->order + 1) - fir->order - 1;

    unsigned int exponent = pk_next2pow(fir->order + 1);
    unsigned int size = 1 << exponent;

    fir->buffer = calloc(size, sizeof(<I>));
    fir->mask = (1 << exponent) - 1;
    fir->index = 0;

    fir->coeff = malloc((fir->order + 1) * sizeof(<O>));
    memcpy(fir->coeff, coeff, (fir->order + 1) * sizeof(<O>));

    return fir;
}

void pk_fir_XX_push(pk_fir_XX *fir, <I> item)
{
    fir->buffer[(fir->index++) & fir->mask] = item;
}

void pk_fir_XX_load(pk_fir_XX *fir, const <O> *coeff)
{
    memcpy(fir->coeff, coeff, (fir->order + 1) * sizeof(<O>));
}

void pk_fir_XX_execute(pk_fir_XX *fir, <O> *output, const <I> *samples, size_t size)
{
    size_t i, j;
    for (i = 0; i < size; i++) {
        pk_fir_XX_push(fir, samples[i]);
        output[i] = 0;
        for (j = 0; j <= fir->order; j++) {
            size_t index = (j + fir->diff + fir->index) & fir->mask;
            output[i] += (fir->buffer[index] * fir->coeff[fir->order - j]);
        }
    }
}

void pk_fir_XX_destroy(pk_fir_XX *fir)
{
    free(fir->coeff);
    free(fir->buffer);
    free(fir);
}


/* Second-order IIR filter:
 * modified version of direct form I */
typedef struct pk_iirso_XX_s
{
    <I> *buffer;
    <O> *a, *b;
    unsigned int mask;
    unsigned int index;
} pk_iirso_XX;

pk_iirso_XX *pk_iirso_XX_create(const <O> *a, const <O> *b)
{
    pk_iirso_XX *iir = malloc(sizeof(pk_iirso_XX));
    iir->buffer = calloc(2, sizeof(<I>));

    iir->a = malloc(3 * sizeof(<O>));
    iir->b = malloc(3 * sizeof(<O>));
    memcpy(iir->a, a, 3 * sizeof(<O>));
    memcpy(iir->b, b, 3 * sizeof(<O>));

    iir->mask = 1;
    iir->index = 0;

    return iir;
}

void pk_iirso_XX_load(pk_iirso_XX *iir, const <O> *a, const <O> *b)
{
    memcpy(iir->a, a, 3 * sizeof(<O>));
    memcpy(iir->b, b, 3 * sizeof(<O>));
}

void pk_iirso_XX_push(pk_iirso_XX *iir, <I> item)
{
    iir->buffer[(iir->index++) & iir->mask] = item;
}

void pk_iirso_XX_execute(pk_iirso_XX *iir, <O> *output, const <I> *samples, size_t size)
{
    size_t i;
    for (i = 0; i < size; i++) {
        <I> delay1 = iir->buffer[(1 + iir->index) & iir->mask];
        <I> delay2 = iir->buffer[(0 + iir->index) & iir->mask];

        // feedback
        <O> feedback = iir->a[0] * samples[i] - iir->a[1] * delay1 - iir->a[2] * delay2;

        // feedforward
        <O> feedforward = iir->b[0] * feedback + iir->b[1] * delay1 + iir->b[2] * delay2;

        // push the latest feedforward data into the buffer
        pk_iirso_XX_push(iir, feedback);

        output[i] = feedforward;
    }
}

void pk_iirso_XX_destroy(pk_iirso_XX *iir)
{
    free(iir->a);
    free(iir->b);

    free(iir->buffer);
    free(iir);
}
