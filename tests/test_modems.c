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

int test_modem_cfsk_1200()
{
    unsigned int nbits = 256;
    unsigned int samp_sym = 32;
    unsigned int baud = 1200;
    float mark_freq = 1200;
    float space_freq = 2200;

    // generate random bits
    srand(time(NULL));
    int r = rand();
    unsigned char rbits[nbits];

    unsigned int i;
    for (i = 0; i < nbits; i++)
        rbits[i] = (r >> i) & 1;

    // assume the first bit is a zero
    rbits[0] = 0;

    // create the fsk objects
    pk_bfskmod *mod = pk_bfskmod_create(samp_sym, baud, mark_freq, space_freq);
    pk_bfskdemod *demod = pk_bfskdemod_create(samp_sym, baud, mark_freq, space_freq);

    complex float symbols[samp_sym * nbits];
    pk_bfskmod_process(mod, symbols, rbits, nbits);
    pk_bfskdemod_process(demod, symbols, samp_sym * nbits);

    size_t nitems = 0;
    unsigned char *output = pk_bfskdemod_read(demod, &nitems);

    for (i = 0; i < nbits; i++) {
        if (output[i] != rbits[i])
            return FAIL;
    }

    pk_bfskmod_destroy(mod);
    pk_bfskdemod_destroy(demod);

    printf("test_modem_cfsk_1200 passed.\n");
    return PASS;
}

int test_modem_cfsk_9600()
{
    unsigned int nbits = 256;
    unsigned int samp_sym = 32;
    unsigned int baud = 9600;
    float mark_freq = 4800;
    float space_freq = 9200;

    // generate random bits
    srand(time(NULL));
    int r = rand();
    unsigned char rbits[nbits];

    unsigned int i;
    for (i = 0; i < nbits; i++)
        rbits[i] = (r >> i) & 1;

    // assume the first bit is a zero
    rbits[0] = 0;

    // create the fsk objects
    pk_bfskmod *mod = pk_bfskmod_create(samp_sym, baud, mark_freq, space_freq);
    pk_bfskdemod *demod = pk_bfskdemod_create(samp_sym, baud, mark_freq, space_freq);

    complex float symbols[samp_sym * nbits];
    pk_bfskmod_process(mod, symbols, rbits, nbits);
    pk_bfskdemod_process(demod, symbols, samp_sym * nbits);

    size_t nitems = 0;
    unsigned char *output = pk_bfskdemod_read(demod, &nitems);

    for (i = 0; i < nbits; i++) {
        if (output[i] != rbits[i])
            return FAIL;
    }

    pk_bfskmod_destroy(mod);
    pk_bfskdemod_destroy(demod);

    printf("test_modem_cfsk_9600 passed.\n");
    return PASS;
}

int main(int argc, char *argv[])
{
    int result = PASS;

    // run all of the tests
    result += test_modem_cfsk_1200();
    result += test_modem_cfsk_9600();

    printf("all modem tests finished.\n");
    return result;
}
