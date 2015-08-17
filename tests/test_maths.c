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

int test_polynomial_2nd_solve()
{
    size_t n = 2;
    complex float a[3] = {1, 0, -1};

    complex float expect[2] = {-1.00000 + 0.00000 * I,  1.00000 - 0.00000 * I};

    pk_polynomial_solve_madsen(a, n);

    size_t i, j;
    for (i = 0; i < n; i++) {
        int result = FAIL;
        for (j = 0; j < n; j++) {
            if (COMPARE_COMPLEX_DELTA(a[n - i], expect[j]))
                result = PASS;
        }

        if (result != PASS)
            return FAIL;
    }

    printf("test_polynomial_2nd_solve passed.\n");
    return PASS;
}

int test_polynomial_7th_solve()
{
    size_t n = 7;
    complex float a[8] = {1, 1, 1, 1, 1, 1, 1, 1};

    complex float expect[7] = {-1.00000 + 0.00000 * I,  0.00000 - 1.00000 * I,
                                0.00000 + 1.00000 * I,  0.70711 + 0.70711 * I,
                               -0.70711 - 0.70711 * I, -0.70711 + 0.70711 * I,
                                0.70711 - 0.70711 * I};

    pk_polynomial_solve_madsen(a, n);

    size_t i, j;
    for (i = 0; i < n; i++) {
        int result = FAIL;
        for (j = 0; j < n; j++) {
            if (COMPARE_COMPLEX_DELTA(a[n - i], expect[j]))
                result = PASS;
        }

        if (result != PASS)
            return FAIL;
    }

    printf("test_polynomial_7th_solve passed.\n");
    return PASS;
}

int test_polynomial_sort_close()
{
    size_t n = 3;
    complex float a[4] = {1, -6, 11, -6};

    float expect[3] = {1, 2, 3};

    pk_polynomial_solve_madsen(a, n);
    pk_polynomial_sort_poles(a, 0, n);

    size_t i;
    for (i = 0; i < n; i++) {
        if (!COMPARE_DELTA(crealf(a[i + 1]), expect[i]))
            return FAIL;
    }

    printf("test_polynomial_sort_close passed.\n");
    return PASS;
}

int test_polynomial_sort_far()
{
    size_t n = 3;
    complex float a[4] = {1, -6, 11, -6};

    float expect[3] = {1, 2, 3};

    pk_polynomial_solve_madsen(a, n);
    pk_polynomial_sort_poles(a, 1, n);

    size_t i;
    for (i = 0; i < n; i++) {
        if (!COMPARE_DELTA(crealf(a[i + 1]), expect[n - i - 1]))
            return FAIL;
    }

    printf("test_polynomial_sort_far passed.\n");
    return PASS;
}

int main(int argc, char *argv[])
{
    int result = PASS;

    // run all of the tests
    result += test_polynomial_2nd_solve();
    result += test_polynomial_7th_solve();
    result += test_polynomial_sort_close();
    result += test_polynomial_sort_far();

    printf("all maths tests finished.\n");
    return result;
}
