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

/*
 * Portable and efficient mathematical operations.
 */

// compute the Q function
float pk_qfunc(float num)
{
    return 0.5*erfcf(num / M_SQRT1_2);
}

// compute the distance between two complex numbers
float pk_dist_cf(float complex a, float complex b)
{
    float result = cabsf(a) - cabsf(b);
    return result < 0 ? -1.0f * result : result;
}

// find the next power of 2 of a given number
unsigned int pk_next2pow(unsigned int num)
{
    if (num == 0)
        return 2;

    size_t count = 0;
    while (num != 0) {
        num >>= 1;
        count++;
    }

    return count;
}

// find the next power of 2 given a number
// and then raise it to the power of 2
unsigned int pk_next2pow2(unsigned int num)
{
    if (num == 0)
        return 2;

    size_t count = 0;
    while (num != 0) {
        num >>= 1;
        count++;
    }

    return (1 << count);
}

/* Kaj Madsen's polynomial root finder based Newton's method */
// a subroutine for the madsen algorithm
static float poly_b(
    float complex z,
    float complex *fz,
    const float complex *a,
    size_t n)
{
    *fz = a[0];
    size_t i;
    for (i = 1; i <= n; i++)
        *fz = *fz*z + a[i];

    return crealf(*fz) * crealf(*fz) + cimagf(*fz) * cimagf(*fz);
}

// Madsen root solver
// TODO: Rewrite this algorithm
void pk_polynomial_solve_madsen(float complex *a, size_t n)
{
    int j;
    float r, u, min, f, fa, fo, ro, eps, f2, ff;
    float complex zo, foz, z, dz, f1z, fz, v, w, fw;
    float complex a1[n - 1];

    int stage1, div2;
    while (a[n] == 0) n--;
    while (n > 2) {
        size_t i;
        for (i = 0; i < n; i++) a1[i] = a[i] * (n - i);

        r = cabsf(a[n]);
        min = expf((logf(r) - logf(cabsf(a[0]))) / n);
        for (i = 1; i < n; i++) {
            if (a[i] != 0) {
                u = expf((logf(r) - logf(cabsf(a[i])))/(n - i));
                if (u < min) min = u;
            }
        }

        zo = 0;
        ff = fo = crealf(a[n]) * crealf(a[n]) + cimagf(a[n]) * cimagf(a[n]);
        foz = a[n - 1];
        z = a[n - 1] == 0 ? 1.0f : -1.0f * a[n]/a[n-1];
        dz = z = z / cabsf(z) * min / 2.0f;
        f = poly_b(z, &fz, a, n);

        ro = 2.5f * min;
        eps = 4.0f * n * n * 4.93e-32f * fo;

        while ((z + dz) != z && f > eps) {
            u = poly_b(z, &f1z, a1, n - 1);
            if (u == 0) {
                dz = dz * 5.0f * (0.6f + I*0.8f);
            } else {
                dz = fz / f1z;

                v = (foz - f1z)/(zo - z);
                f2 = crealf(v) * crealf(v) + cimagf(v) * cimagf(v);
                stage1 = ((f2/u) > (u/f/4.0f)) || (f != ff);
                r = cabsf(dz);
                if (r>ro) {
                    dz = dz*(ro/r)*(0.6f + I*0.8f);
                    ro = 5.0f * r;
                }
            }
            zo = z; fo = f; foz = f1z;

            iter2:
            z = w = zo - dz;
            ff = f = poly_b(z, &fz, a, n);
            if (stage1) {
                j = 1;
                div2 = f > fo;
                while (j <= n) {
                    if (div2) {
                        dz *= 0.5f;
                        w = zo - dz;
                    } else {
                        w = w - dz;
                    }

                    fa = poly_b(w, &fw, a, n);
                    if (fa < f) {
                        f = fa; fz = fw; z = w; j++;
                        if (div2 && j == 3) {
                            dz = dz * (0.6f + I*0.8f);
                            z = zo - dz;
                            f = poly_b(z, &fz, a, n);
                            j = n + 1;
                        }
                    } else {
                        j = n + 1;
                    }
                }
            }

            if (r < (5.96046e-8f * cabsf(z)) && (f >= fo)) {
                z = zo; dz *= 0.3f + I*0.4f;
                if ((z + dz) != z)
                    goto iter2;
            }
        }

        v = 0;
        for (i = 0; i < n; i++)
            v = a[i] = v * z + a[i];

        // TODO: Print some debug about the state of the solver
        a[n] = z;
        n--;
    }

    // Once we've reduced the order, get the rest of the roots
    if (n == 1) {
        a[1] = -1.0f * a[1] / a[0];
    } else {
        if (a[1] == 0) {
            a[1] = -1.0f * csqrtf(-1.0f * a[2]/a[0]);
            a[2] = -1.0f * a[1];
        } else {
            v = csqrtf((1.0f - 4.0f * a[0] * a[2] / (a[1] * a[1])));
            if (creal(v) < 0)
                a[1] = (-1.0f - v) * a[1] / (2.0f * a[0]);
            else
                a[1] = (-1.0f + v) * a[1] / (2.0f * a[0]);
            a[2] = a[2] / (a[0] * a[1]);
        }
    }
}

// comparison function for sorting from closest
// to furthest from the unit circle
static int sort_close(const void *p, const void *q)
{
    const float complex *a = p, *b = q;
    float p_dist = fabs(cabsf(*a) - 1.0f);
    float q_dist = fabs(cabsf(*b) - 1.0f);

    if (p_dist == q_dist) return 0;

    return p_dist < q_dist ? -1 : 1;
}

// comparison function for sorting from furthest
// to closest from the unit circle
static int sort_far(const void *p, const void *q)
{
    const float complex *a = p, *b = q;
    float p_dist = fabs(cabsf(*a) - 1.0f);
    float q_dist = fabs(cabsf(*b) - 1.0f);

    if (p_dist == q_dist) return 0;

    return p_dist > q_dist ? -1 : 1;
}

/* Bairstow's method for polynomial root finding */
void pk_polynomial_solve_bairstow(float complex *a, int order, size_t n)
{
    //TODO: Be sure to compare with Madsen's algorithm above.
}

// sort the poles by their proximity to the unit circle
void pk_polynomial_sort_poles(float complex *b, int order, size_t n)
{
    int (*sort_ptr)(const void *, const void *);
    sort_ptr = order == 0 ? sort_close : sort_far;

    qsort(&b[1], n, sizeof(float complex), sort_ptr);
}

typedef struct zero_sort_s
{
    float complex ref;
    float complex value;
} zero_sort;

// sort the zero / pole pairs
static int sort_val_close(const void *p, const void *q)
{
    const zero_sort *a = p, *b = q;
    float p_dist = cabsf(a->value - a->ref);
    float q_dist = cabsf(b->value - b->ref);

    if (p_dist == q_dist) return 0;

    return p_dist < q_dist ? -1 : 1;
}

// sort the zeros by their proxmity to each pole
void pk_polynomial_sort_zeros(float complex *a, const float complex *b, size_t n)
{
    zero_sort zeros[n];

    size_t i, j;
    for (i = 1; i < n; i++) {
        for (j = 1; j < n; j++) {
           zeros[j].value = a[j];
           zeros[j].ref = b[i];
        }

        qsort(&zeros[i], n - i, sizeof(zero_sort), sort_val_close);

        for (j = 1; j < n; j++)
            a[j] = zeros[j].value;
    }
}
