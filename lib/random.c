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

/* arbitrary additive data scrambler/descrambler */
// serves both as a scrambler and descrambler
typedef struct pk_add_scrambler_s
{
    pk_lfsr *lfsr;
} pk_add_scrambler;

pk_add_scrambler *pk_add_scrambler_create(unsigned int n, uint32_t start)
{
    if (n > 19 || n < 2) {
        printf("pk_add_scrambler error: n must be between 2 and 19\n");
        exit(1);
    }

    pk_add_scrambler *as = malloc(sizeof(pk_add_scrambler));

    as->lfsr = pk_lfsr_create(n, start);

    return as;
}

unsigned char pk_add_scrambler_execute(pk_add_scrambler *as, unsigned char input)
{
    uint32_t state = pk_lfsr_execute(as->lfsr, NULL);
    return (state + input) & 1;
}

void pk_add_scrambler_destroy(pk_add_scrambler *as)
{
    pk_lfsr_destroy(as->lfsr);

    free(as);
}

/* arbitrary multiplicative data scrambler */
// has a corresponding descrambler with slightly different structure
typedef struct pk_mult_scrambler_s
{
    uint32_t start;
    uint32_t state;
    uint32_t poly;
} pk_mult_scrambler;

pk_mult_scrambler *pk_mult_scrambler_create(unsigned int n, uint32_t start)
{
    if (n > 19 || n < 2) {
        printf("pk_mult_scrambler error: n must be between 2 and 19\n");
        exit(1);
    }

    pk_mult_scrambler *ms = malloc(sizeof(pk_mult_scrambler));

    ms->start = start;
    ms->state = start;
    ms->poly = lfsr_poly_tab[19 - n] >> 1;

    return ms;
}

unsigned char pk_mult_scrambler_execute(pk_mult_scrambler *ms, unsigned char input)
{
    uint32_t bit = (ms->state ^ input) & 1;
    ms->state >>= 1;
    ms->state ^= ms->poly & -bit;

    return bit;
}

void pk_mult_scrambler_destroy(pk_mult_scrambler *ms)
{
    free(ms);
}

/* arbitrary multiplicative data descrambler */
typedef struct pk_mult_descrambler_s
{
    uint32_t start;
    uint32_t state;
    uint32_t poly;
} pk_mult_descrambler;

pk_mult_descrambler *pk_mult_descrambler_create(unsigned int n, uint32_t start)
{
    if (n > 19 || n < 2) {
        printf("pk_mult_descrambler error: n must be between 2 and 19\n");
        exit(1);
    }

    pk_mult_descrambler *md = malloc(sizeof(pk_mult_descrambler));

    md->start = start;
    md->state = start;
    md->poly = lfsr_poly_tab[19 - n] >> 1;

    return md;
}

unsigned char pk_mult_descrambler_execute(pk_mult_descrambler *md, unsigned char input)
{
    uint32_t bit = input & 1;
    uint32_t out = (input ^ md->state) & 1;
    md->state >>= 1;
    md->state ^= md->poly & -bit;

    return out;
}

void pk_mult_descrambler_destroy(pk_mult_descrambler *md)
{
    free(md);
}
