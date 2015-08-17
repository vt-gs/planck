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

int test_buffer_fill()
{
    // generate random bits
    srand(time(NULL));
    int r = rand();
    unsigned char rbits[32];

    pk_circ_uu * buffer = pk_circ_uu_create(32);
    size_t i;
    for (i = 0; i < 32; i++) {
        rbits[i] = (r >> i) & 1;
        pk_circ_uu_push(buffer, rbits[i]);
    }

    unsigned char out[32];
    pk_circ_uu_read(buffer, out, 32);

    for (i = 0; i < 32; i++) {
        if (rbits[i] != out[i]) {
            pk_circ_uu_destroy(buffer);
            return FAIL;
        }
    }

    pk_circ_uu_destroy(buffer);
    printf("test_buffer_fill passed.\n");
    return PASS;
}

int test_buffer_overfill()
{
    // generate random bits
    srand(time(NULL));
    int r = rand();
    unsigned char rbits[32];

    pk_circ_uu * buffer = pk_circ_uu_create(16);
    size_t i;
    for (i = 0; i < 32; i++) {
        rbits[i] = (r >> i) & 1;
        pk_circ_uu_push(buffer, rbits[i]);
    }

    unsigned char out[32];
    pk_circ_uu_read(buffer, out, 32);

    for (i = 0; i < 16; i++) {
        if (rbits[16 + i] != out[i]) {
            pk_circ_uu_destroy(buffer);
            return FAIL;
        }
    }

    pk_circ_uu_destroy(buffer);
    printf("test_buffer_overfill passed.\n");
    return PASS;
}

int test_buffer_sortof_fill()
{
    // generate random bits
    srand(time(NULL));
    int r = rand();
    unsigned char rbits[32];

    pk_circ_uu * buffer = pk_circ_uu_create(32);
    size_t i;
    for (i = 0; i < 30; i++) {
        rbits[i] = (r >> i) & 1;
        pk_circ_uu_push(buffer, rbits[i]);
    }

    unsigned char out[32];
    pk_circ_uu_read(buffer, out, 32);

    for (i = 0; i < 32; i++) {
        if (rbits[i] != out[i]) {
            pk_circ_uu_destroy(buffer);
            return PASS;
        }
    }

    pk_circ_uu_destroy(buffer);
    printf("test_buffer_sortof_fill passed.\n");
    return FAIL;
}

int test_buffer_pop()
{
    // generate random bits
    srand(time(NULL));
    int r = rand();
    unsigned char rbits[32];

    pk_circ_uu *buffer = pk_circ_uu_create(32);
    size_t i;
    for (i = 0; i < 32; i++) {
        rbits[i] = (r >> i) & 1;
        pk_circ_uu_push(buffer, rbits[i]);
    }

    for (i = 0; i < 5; i++) {
        if (rbits[32 - i - 1] != pk_circ_uu_pop(buffer)) {
            pk_circ_uu_destroy(buffer);
            return FAIL;
        }
    }

    pk_circ_uu_destroy(buffer);
    printf("test_buffer_pop passed.\n");
    return PASS;
}

int test_buffer_read()
{
    complex float symbols[5] = {1 + 1*_Complex_I, 2 + 1*_Complex_I,
                                3 + 1*_Complex_I, 4 + 1*_Complex_I,
                                5 + 1*_Complex_I};

    pk_circ_cc * buffer = pk_circ_cc_create(4);

    size_t i;
    for (i = 0; i < 5; i++)
        pk_circ_cc_push(buffer, symbols[i]);


    complex float out[4];
    pk_circ_cc_read(buffer, out, 4);
    pk_circ_cc_destroy(buffer);

    for (i = 0; i < 4; i++) {
        if (out[i] != symbols[i+1])
            return FAIL;
    }

    printf("test_buffer_read passed.\n");
    return PASS;
}

int test_block_read()
{
    // generate random bits
    srand(time(NULL));
    int r = rand();
    unsigned char rbits[32];

    pk_block_uu *block = pk_block_uu_create(2);

    size_t i;
    for (i = 0; i < 32; i++) {
        rbits[i] = (r >> i) & 1;
        pk_block_uu_push(block, rbits[i]);
    }

    unsigned char *out;
    size_t nitems = pk_block_uu_nitems(block);
    out = pk_block_uu_ptr(block);

    if (nitems != 32)
        return FAIL;

    for (i = 0; i < 32; i++) {
        if (rbits[i] != out[i]) {
            return FAIL;
        }
    }

    pk_block_uu_destroy(block);

    printf("test_block_read passed.\n");
    return PASS;
}

int test_queue_read()
{
    // generate random bits
    srand(time(NULL));
    int r = rand();
    unsigned char rbits[32];

    pk_queue_uu * queue = pk_queue_uu_create();
    size_t i;
    for (i = 0; i < 32; i++) {
        rbits[i] = (r >> i) & 1;
        pk_queue_uu_insert(queue, rbits[i]);
    }

    unsigned char out[32];
    pk_queue_uu_read(queue, out, 32);

    for (i = 0; i < 32; i++) {
        if (rbits[i] != out[i]) {
            return FAIL;
        }
    }

    pk_queue_uu_destroy(queue);

    printf("test_queue_read passed.\n");
    return PASS;
}

int test_queue_partial_read()
{
    // generate random bits
    srand(time(NULL));
    int r = rand();
    unsigned char rbits[32];

    pk_queue_uu * queue = pk_queue_uu_create();

    size_t i;
    for (i = 0; i < 32; i++) {
        rbits[i] = (r >> i) & 1;
        pk_queue_uu_insert(queue, rbits[i]);
    }

    for (i = 0; i < 16; i++)
        pk_queue_uu_dequeue(queue);

    if (pk_queue_uu_nitems(queue) != 16)
        return FAIL;

    unsigned char out[16];
    pk_queue_uu_read(queue, out, 16);

    for (i = 0; i < 16; i++) {
        if (rbits[i + 16] != out[i]) {
            return FAIL;
        }
    }

    pk_queue_uu_destroy(queue);

    printf("test_queue_partial_read passed.\n");
    return PASS;
}

int main(int argc, char *argv[])
{
    int result = PASS;

    // run all of the tests
    result += test_buffer_fill();
    result += test_buffer_overfill();
    result += test_buffer_sortof_fill();
    result += test_buffer_pop();
    result += test_buffer_read();
    result += test_block_read();
    result += test_queue_read();
    result += test_queue_partial_read();

    printf("all buffer tests finished.\n");
    return result;
}
