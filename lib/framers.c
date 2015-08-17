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

/* AX.25 framing object */
typedef struct pk_ax25_framer_s
{
    unsigned int padding;
    unsigned int count;
    pk_block_uu *frame;
} pk_ax25_framer;

pk_ax25_framer *pk_ax25_framer_create(unsigned int padding)
{
    pk_ax25_framer *f = malloc(sizeof(pk_ax25_framer));

    f->frame = pk_block_uu_create(8 * MAX_AX25_BYTES);
    f->padding = padding;
    f->count = 0;

    return f;
}

static void print_ax25_framer(pk_ax25_framer *f)
{
    unsigned char *data = pk_block_uu_ptr(f->frame);
    unsigned char frame_size = pk_block_uu_nitems(f->frame);

    printf("***** pk_ax25_framer_process debug *****\n");
    printf("    frame size: %d bytes\n", frame_size);
    printf("    framer bit stream:\n");

    size_t i, j;
    for (i = 0; i < frame_size / 8 + 1; i++) {
        printf("        ");
        for (j = 0; j < 8; j++)
            printf("%d", (int) data[8*i + j]);
        printf("\n");
    }
    printf("    end stream.\n");
}

static void ax25_insert_pad(pk_ax25_framer *f)
{
    size_t i;
    for (i = 0; i < f->padding; i++)
        pk_block_uu_push(f->frame, 0);
}

static void ax25_insert_flag(pk_ax25_framer *f)
{
    size_t i;
    for (i = 0; i < 8; i++)
        pk_block_uu_push(f->frame, (AX25_FLAG >> i) & 1);
}

void pk_ax25_framer_process(
    pk_ax25_framer *f,
    const unsigned char *bytes,
    size_t size)
{
    pk_block_uu_clear(f->frame);

    unsigned int crc;
    unsigned char crc_bytes[2] = {0};

    crc  = crc_ax25_byte(bytes, size);
    crc ^= 0xffff;
    crc_bytes[0] |= (crc & 0x00ff);
    crc_bytes[1] |= (crc & 0xff00) >> 8;

    ax25_insert_pad(f);
    ax25_insert_flag(f);

    size_t i, j, c = 0;
    for (i = 0; i < size + 2; i++) {
        unsigned char bits[8];

        if (i < size)
            pk_unpack_byte_rl(bits, bytes[i]);
        else
            pk_unpack_byte_rl(bits, crc_bytes[c++]);

        for (j = 0; j < 8; j++) {
            pk_block_uu_push(f->frame, bits[j]);
            f->count = bits[j] & 1 ? ++f->count : 0;

            // bit stuff after we've seen 5 ones
            if (f->count == 5) {
                pk_block_uu_push(f->frame, 0);
                f->count = 0;
            }
        }
    }

    ax25_insert_flag(f);
    ax25_insert_pad(f);

#if PK_DEBUG == VERBOSE
    print_ax25_framer(f);
#endif
}

unsigned char *pk_ax25_framer_read(pk_ax25_framer *f, size_t *out_nitems)
{
    if (out_nitems != NULL)
        *out_nitems = pk_block_uu_nitems(f->frame);

    return pk_block_uu_ptr(f->frame);
}

void pk_ax25_framer_destroy(pk_ax25_framer *f)
{
    pk_block_uu_destroy(f->frame);
    free(f);
}

typedef enum {
    AX25_DETECT=0,
    AX25_DECODE
} ax25_mode;

/* AX.25 deframing object */
typedef struct pk_ax25_deframer_s
{
    ax25_mode state;
    unsigned int count;

    void *info;
    void (*callback)(int valid, unsigned char *payload, void *info, size_t size);

    pk_block_uu *data;
    pk_block_uu *packed;
    pk_circ_uu *window;
    pk_circ_uu *buffer;
} pk_ax25_deframer;

pk_ax25_deframer *pk_ax25_deframer_create(
    void *info,
    void (*callback_ptr)(int valid, unsigned char *payload, void *info, size_t size))
{
    pk_ax25_deframer *df = malloc(sizeof(pk_ax25_deframer));
    df->info = info;
    df->callback = callback_ptr;

    df->state = AX25_DETECT;
    df->count = 0;

    df->data = pk_block_uu_create(8 * MAX_AX25_BYTES);
    df->packed = pk_block_uu_create(MAX_AX25_BYTES);
    df->window = pk_circ_uu_create(8);
    df->buffer = pk_circ_uu_create(8);

    return df;
}

static void ax25_unstuff_bits(pk_ax25_deframer *df)
{
    // reset everything
    pk_block_uu_clear(df->packed);
    pk_circ_uu_clear(df->buffer);

    // subtract end of the flag
    size_t size = pk_block_uu_nitems(df->data);
    assert(size > 7);

    size_t count = 0;
    size_t ones  = 0;

    unsigned char *data = pk_block_uu_ptr(df->data);

    size_t i;
    for (i = 0; i < size - 7; i++) {
        if (ones < 5) {
            pk_circ_uu_push(df->buffer, data[i]);
            count++;
        }

        ones = data[i] & 1 ? ++ones : 0;

        if (count == 8) {
            unsigned char input[8];
            pk_circ_uu_read(df->buffer, input, 8);
            unsigned char byte = pk_pack_byte_rl(input);
            pk_block_uu_push(df->packed, byte);
            count = 0;
        }
    }
}

void pk_ax25_deframer_process(
    pk_ax25_deframer *df,
    const unsigned char *bits,
    size_t size)
{
    size_t i;
    for (i = 0; i < size; i++) {
        unsigned char input[8];
        pk_circ_uu_push(df->window, bits[i]);
        pk_circ_uu_read(df->window, input, 8);
        unsigned char byte = pk_pack_byte_rl(input);

        if (df->state == AX25_DETECT) {
            if (byte == AX25_FLAG) {

#if PK_DEBUG == VERBOSE
                printf("***** pk_ax25_deframer_process debug ******\n");
                printf("    Detected AX.25 frame.\n");
#endif

                df->state = AX25_DECODE;
                df->count = 0;
                pk_circ_uu_clear(df->window);
            }
        } else if (df->state == AX25_DECODE) {
            if (df->count > 8 * MAX_AX25_BYTES) {

                df->state = AX25_DETECT;
#if PK_DEBUG == VERBOSE
                printf("***** pk_ax25_deframer_process debug ******\n");
                printf("    AX.25 frame exceeded defined maximum of\n");
                printf("    %d bytes.\n", MAX_AX25_BYTES);
#endif
                pk_block_uu_clear(df->data);
                pk_block_uu_clear(df->packed);

            } else if (byte == AX25_FLAG) {

                df->state = AX25_DETECT;

                if (df->count > MIN_AX25_BYTES) {

                    // unstuff the received frame
                    ax25_unstuff_bits(df);

                    size_t frame_size = pk_block_uu_nitems(df->packed);
                    unsigned char *frame_data = pk_block_uu_ptr(df->packed);

                    // calculate the CRC and check for the magic number
                    unsigned int result = crc_ax25_byte(frame_data, frame_size);
                    int valid = result == AX25_CRC_MAGIC;

#if PK_DEBUG == VERBOSE
                    printf("***** pk_ax25_deframer_process debug ******\n");
                    printf("    Detected end of an AX.25 frame.\n");
                    printf("    CRC calculation :    %d\n", result);
                    printf("    CRC magic calc  :    %d\n", AX25_CRC_MAGIC);
                    printf("    Frame ASCII     :\n");

                    size_t r, c;
                    for (r = 0; r < frame_size / 8 + 1; r++) {
                        for (c = 0; c < 8; c++) {
                            if (8 * r + c < frame_size)
                                printf("    %c", frame_data[8*r+c]);
                        }
                        printf("\n");
                    }
#endif

                    // pass payload to a callback
                    df->callback(valid, frame_data, df->info, frame_size);
                    df->state = AX25_DETECT;
                }

                pk_block_uu_clear(df->data);
                pk_block_uu_clear(df->packed);

            } else {
                pk_block_uu_push(df->data, bits[i]);
                df->count++;
            }
        }
    }
}

void pk_ax25_deframer_destroy(pk_ax25_deframer *df)
{
    pk_block_uu_destroy(df->data);
    pk_block_uu_destroy(df->packed);
    pk_circ_uu_destroy(df->window);
    pk_circ_uu_destroy(df->buffer);

    free(df);
}
