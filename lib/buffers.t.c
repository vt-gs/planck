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

/* Generic circular buffer */
typedef struct pk_circ_XX_s
{
    <I> *buffer;
    size_t buf_size;
    unsigned int mask;
    unsigned int diff;
    unsigned int count;
    unsigned int index;
} pk_circ_XX;

pk_circ_XX *pk_circ_XX_create(unsigned int size)
{
    pk_circ_XX *cb = malloc(sizeof(pk_circ_XX));
    unsigned int exponent = pk_next2pow(size);
    unsigned int new_size = pk_next2pow2(size);

    cb->diff = new_size - size;
    assert(cb->diff >= 0);

    cb->buf_size = new_size;
    cb->buffer = calloc(cb->buf_size, sizeof(<I>));
    cb->mask = (1 << exponent) - 1;
    cb->count = 0;
    cb->index = 0;

    return cb;
}

void pk_circ_XX_push(pk_circ_XX *cb, <I> item)
{
    cb->buffer[(cb->index++) & cb->mask] = item;
    cb->count = cb->count > cb->buf_size ? 0 : cb->count + 1;
}

void pk_circ_XX_read(pk_circ_XX *cb, <O> *output, size_t num)
{
    if (num > cb->buf_size || num == 0)
        num = cb->buf_size;

    size_t i;
    for (i = 0; i < num; i++)
        output[i] = cb->buffer[(i + cb->diff + cb->index) & cb->mask];
}

<O> pk_circ_XX_pop(pk_circ_XX *cb)
{
    if (cb->count < 1) {
        fprintf(stderr, "circular buffer is empty.\n");
        exit(EXIT_FAILURE);
    }

    cb->count--;
    return cb->buffer[--cb->index & cb->mask];
}

void pk_circ_XX_clear(pk_circ_XX *cb)
{
    cb->count = 0;
    cb->index = 0;
}

void pk_circ_XX_destroy(pk_circ_XX *cb)
{
    free(cb->buffer);
    free(cb);
}


/* Generic dynamic memory block for variable output */
typedef struct pk_block_XX_s
{
    size_t size;
    size_t nitems;
    <O> *output;
} pk_block_XX;

pk_block_XX *pk_block_XX_create(size_t size)
{
    pk_block_XX *b = malloc(sizeof(pk_block_XX));
    b->size = size;
    b->nitems = 0;
    b->output = calloc(size, sizeof(<O>));
    return b;
}

void pk_block_XX_resize(pk_block_XX *b, size_t new_size)
{
    <O> *new_output;
    new_output = realloc(b->output, new_size * sizeof(<O>));

    if (!new_output) {
        free(b->output);
        fprintf(stderr, "unable to reallocate a new block size!\n");
        exit(1);
    }

    b->output = new_output;
    b->size = new_size;
}

<O> *pk_block_XX_ptr(pk_block_XX *b)
{
    return b->output;
}

void pk_block_XX_push(pk_block_XX *b, <I> item)
{
    while (b->nitems > b->size)
        pk_block_XX_resize(b, 2*b->size);

    b->output[b->nitems++] = item;
}

size_t pk_block_XX_nitems(pk_block_XX *b)
{
    return b->nitems;
}

size_t pk_block_XX_size(pk_block_XX *b)
{
    return b->size;
}

void pk_block_XX_clear(pk_block_XX *b)
{
    b->nitems = 0;
}

void pk_block_XX_destroy(pk_block_XX *b)
{
    free(b->output);
    free(b);
}


/* Generic data queue using a linked list*/
// TODO: Improve efficiency by pre-allocating a pool
// instead of malloc'ing for each element
typedef struct pk_queue_XX_node_s
{
    <I> value;
    struct pk_queue_XX_node_s *next;
} pk_queue_XX_node;

typedef struct pk_queue_XX_s
{
    size_t nitems;
    pk_queue_XX_node *begin;
    pk_queue_XX_node *curr;
} pk_queue_XX;

pk_queue_XX *pk_queue_XX_create()
{
    pk_queue_XX *q = malloc(sizeof(pk_queue_XX));

    q->nitems = 0;
    q->begin = NULL;
    q->curr = NULL;

    return q;
}

void pk_queue_XX_insert(pk_queue_XX *q, <I> item)
{
    if (q->nitems == 0) {
        q->begin = malloc(sizeof(pk_queue_XX_node));
        q->begin->value = item;
        q->begin->next = NULL;
        q->curr = q->begin;
    } else {
        pk_queue_XX_node *node = malloc(sizeof(pk_queue_XX_node));
        node->value = item;
        node->next = NULL;
        q->curr->next = node;
        q->curr = node;
    }

    q->nitems++;
}

void pk_queue_XX_dequeue(pk_queue_XX *q)
{
    if (q->nitems == 0)
        return;

    if (q->begin->next != NULL) {
        pk_queue_XX_node *node = q->begin->next;
        free(q->begin);
        q->begin = node;
    } else {
        free(q->begin);
    }

    q->nitems--;
}

void pk_queue_XX_clear(pk_queue_XX *q)
{
    while (q->nitems != 0)
        pk_queue_XX_dequeue(q);
}

size_t pk_queue_XX_nitems(pk_queue_XX *q)
{
    return q->nitems;
}

void pk_queue_XX_read(pk_queue_XX *q, <O> *output, size_t num)
{
    if (num > q->nitems || num == 0)
        num = q->nitems;

    pk_queue_XX_node *node = q->begin;

    size_t i;
    for (i = 0; i < num; i++) {
        output[i] = node->value;
        node = node->next;
    }
}

void pk_queue_XX_destroy(pk_queue_XX *q)
{
    pk_queue_XX_clear(q);

    free(q);
}
