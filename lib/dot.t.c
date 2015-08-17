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

/* dot product objects */
// simple dot product
// TODO: include SIMD intrinsics
typedef struct pk_dotprod_XX_s
{
    <O> *seq;
    size_t size;
} pk_dotprod_XX;

pk_dotprod_XX *pk_dotprod_XX_create(const <O> *seq, size_t size)
{
    pk_dotprod_XX *dp = malloc(sizeof(pk_dotprod_XX));

    dp->size = size;
    dp->seq = malloc(size * sizeof(<O>));

    memcpy(dp->seq, seq, size * sizeof(<O>));
}

void pk_dotprod_XX_load(pk_dotprod_XX *dp, const <O> *seq, size_t size)
{
    memcpy(dp->seq, seq, size * sizeof(<O>));
}

<O> pk_dotprod_XX_execute(pk_dotprod_XX *dp, const <I> *in, size_t size)
{
    <O> result = 0;

    unsigned int i;
    for (i = 0; i < size; i++)
        result += (in[i] * conj(dp->seq[i]));

    return result;
}

void pk_dotprod_XX_destroy(pk_dotprod_XX *dp)
{
    free(dp->seq);
    free(dp);
}
