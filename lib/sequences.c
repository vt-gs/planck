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

/* Maximal LFSR generator */
typedef struct pk_lfsr_s
{
    uint32_t start;
    uint32_t state;
    uint32_t poly;
    size_t p;
} pk_lfsr;

pk_lfsr *pk_lfsr_create(unsigned int n, uint32_t start)
{
    if (n > 19 || n < 2) {
        printf("pk_lfsr error: n must be between 2 and 19\n");
        exit(1);
    }

    pk_lfsr *l = malloc(sizeof(pk_lfsr));

    l->p = 0;
    l->start = start;
    l->state = start;
    l->poly  = lfsr_poly_tab[19 - n] >> 1;

    return l;
}

uint32_t pk_lfsr_execute(pk_lfsr *l, int *out_period)
{
    uint32_t bit = l->state & 1;
    l->state >>= 1;
    l->state ^= l->poly & -bit;
    l->p++;

    if (l->state == l->start)
        l->p = 0;

    if (out_period != NULL)
        *out_period = l->p;

    return l->state;
}

void pk_lfsr_destroy(pk_lfsr *l)
{
    free(l);
}
