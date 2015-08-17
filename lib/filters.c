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

/* Second-order section structure definition */
typedef struct pk_iirso_cc_s
{
    float complex *buffer;
    float complex *a, *b;
    unsigned int mask;
    unsigned int index;
} pk_iirso_cc;

/* Higher-order IIR filter based on
 * second-order partitioning. */
typedef struct pk_iir_cascade_s
{
    float complex *a, *b;

    unsigned int order;
    unsigned int cascade;
    unsigned int nsos;

    pk_iirso_cc **sos;
} pk_iir_cascade;

static void sos_solve(pk_iir_cascade *iir)
{
    // create the second-order sections based on the given coefficients
    // solve for poles and zeros
    pk_polynomial_solve_madsen(iir->a, iir->order);
    pk_polynomial_solve_madsen(iir->b, iir->order);

    // sort by poles furthest -> closest to the unit circle
    pk_polynomial_sort_poles(iir->b, iir->cascade, iir->order);

    // sort by zeros furthest -> closest to each pole
    pk_polynomial_sort_zeros(iir->a, iir->b, iir->order);
}

pk_iir_cascade *pk_iir_cascade_create(
    unsigned int order,     // order of the IIR
    unsigned int cascade,   // Sort by closest (0) to furthest (1) to unit circle
    const float complex *a, // numerator coefficients
    const float complex *b) // denominator coefficients
{
    pk_iir_cascade *iir = malloc(sizeof(pk_iir_cascade));
    iir->order = order;
    iir->nsos = iir->order / 2;

    // TODO: should be a enum type
    iir->cascade = cascade;

    assert((iir->order % 2) == 0);

    iir->a = malloc((iir->order + 1) * sizeof(float complex));
    iir->b = malloc((iir->order + 1) * sizeof(float complex));
    memcpy(iir->a, a, (iir->order + 1) * sizeof(float complex));
    memcpy(iir->b, b, (iir->order + 1) * sizeof(float complex));

    // allocate memory for second-order sections
    iir->sos = malloc(iir->nsos * sizeof(pk_iirso_cc));

    sos_solve(iir);

    float complex as[3], bs[3];
    size_t i;
    for (i = 0; i < iir->nsos; i++) {
        as[0] = 1;
        as[1] = -1.0f * iir->a[2*i + 1] - iir->a[2*i + 2];
        as[2] = iir->a[2*i + 1] * iir->a[2*i + 2];

        bs[0] = 1;
        bs[1] = -1.0f * iir->b[2*i + 1] - iir->b[2*i + 2];
        bs[2] = iir->b[2*i + 1] * iir->b[2*i + 2];

        iir->sos[i] = pk_iirso_cc_create(as, bs);
    }

    return iir;
}

void pk_iir_cascade_load(pk_iir_cascade *iir, const float complex *a, const float complex *b)
{
    memcpy(iir->a, a, (iir->order + 1) * sizeof(float complex));
    memcpy(iir->b, b, (iir->order + 1) * sizeof(float complex));

    sos_solve(iir);

    float complex as[3], bs[3];
    size_t i;
    for (i = 0; i < iir->nsos; i++) {
        as[0] = 1;
        as[1] = -1.0f * iir->a[2*i + 1] - iir->a[2*i + 2];
        as[2] = iir->a[2*i + 1] * iir->a[2*i + 2];

        bs[0] = 1;
        bs[1] = -1.0f * iir->b[2*i + 1] - iir->b[2*i + 2];
        bs[2] = iir->b[2*i + 1] * iir->b[2*i + 2];

        pk_iirso_cc_load(iir->sos[i], as, bs);
    }
}

void pk_iir_cascade_execute(pk_iir_cascade *iir, float complex *output, const float complex *samples, size_t size)
{
    pk_iirso_cc_execute(iir->sos[0], output, samples, size);

    size_t i;
    for (i = 1; i < iir->nsos; i++)
        pk_iirso_cc_execute(iir->sos[i], output, output, size);
}

void pk_iir_cascade_destroy(pk_iir_cascade *iir)
{
    size_t i;
    for (i = 0; i < iir->nsos; i++)
        pk_iirso_cc_destroy(iir->sos[i]);

    free(iir->sos);

    free(iir->a);
    free(iir->b);
    free(iir);
}
