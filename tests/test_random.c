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

#include "common.h"

int test_mult_scrambler()
{
    // generate random bits
    srand(time(NULL));
    int r = rand();
    unsigned char rbits[32];
    unsigned char sbits[32];

    unsigned char out[32];

    pk_mult_scrambler *scrambler = pk_mult_scrambler_create(8, 0xFFFF);
    pk_mult_descrambler *descrambler = pk_mult_descrambler_create(8, 0xFFFF);

    size_t i;
    for (i = 0; i < 32; i++) {
        rbits[i] = (r >> i) & 1;
        sbits[i] = pk_mult_scrambler_execute(scrambler, rbits[i]);
        out[i] = pk_mult_descrambler_execute(descrambler, sbits[i]);
    }

    for (i = 0; i < 32; i++) {
        if (rbits[i] != out[i]) {
            return FAIL;
        }
    }

    printf("test_mult_scrambler passed.\n");
    return PASS;
}

int main(int argc, char *argv[])
{
    int result = PASS;

    // run all of the tests
    result += test_mult_scrambler();

    printf("all random tests finished.\n");
    return result;
}
