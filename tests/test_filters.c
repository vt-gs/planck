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

int test_fir_impulse()
{
    unsigned int order = 3;
    float coeff[4]   = {1, 2, 3, 4};
    float samples[5] = {1, 1, 0, 0, 0};
    float expect[5]  = {1, 3, 5, 7, 4};
    float output[5]  = {0, 0, 0, 0, 0};

    pk_fir_ff *filter = pk_fir_ff_create(order, coeff);
    pk_fir_ff_execute(filter, output, samples, 5);

    size_t i;
    for (i = 0; i < 5; i++) {
        if (output[i] != expect[i])
            return FAIL;
    }

    pk_fir_ff_destroy(filter);
    printf("test_fir_impulse passed.\n");
    return PASS;
}

int test_iirso_impulse()
{
    float a[3] = {1, 1, 0.5};
    float b[3] = {1, 2, 3};
    float samples[15] = {1, 0, 0, 0, 0,
                         0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0};

    float expect[15] = { 1.0000,  1.0000, 1.5000, -2.0000,  1.2500,
                        -0.2500, -0.3750, 0.5000, -0.3125,  0.0625,
                         0.0938, -0.1250, 0.0781, -0.0156, -0.0234};

    float output[15] = {0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0};

    pk_iirso_ff *filter = pk_iirso_ff_create(a, b);
    pk_iirso_ff_execute(filter, output, samples, 15);

    size_t i;
    for (i = 0; i < 15; i++) {
        if (!COMPARE_DELTA(output[i], expect[i]))
            return FAIL;
    }


    pk_iirso_ff_destroy(filter);
    printf("test_iirso_impulse passed.\n");
    return PASS;
}

int test_iir_cascade_impulse()
{
    complex float a[7] = {1, 1, 0.5, 0.5, 0.5, 0.5, 0.5};
    complex float b[7] = {1, 2, 3, 4, 5, 6, 7};
    complex float samples[6] = {1, 0, 0, 0, 0, 0};

    complex float expect[6] = {1.0000,  1.0000, 1.5000, 1.5000,  1.7500, 1.7500};
    complex float output[6] = {0, 0, 0, 0, 0, 0};

    pk_iir_cascade *filter = pk_iir_cascade_create(6, 0, a, b);
    pk_iir_cascade_execute(filter, output, samples, 6);

    size_t i;
    for (i = 0; i < 6; i++) {
        if (!COMPARE_COMPLEX_DELTA(output[i], expect[i]))
            return FAIL;
    }


    pk_iir_cascade_destroy(filter);
    printf("test_iir_cascade_impulse passed.\n");
    return PASS;
}

int main(int argc, char *argv[])
{
    int result = PASS;

    // run all of the tests
    result += test_fir_impulse();
    result += test_iirso_impulse();
    result += test_iir_cascade_impulse();

    printf("all filter tests finished.\n");
    return result;
}
