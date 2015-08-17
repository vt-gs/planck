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

int frame_matches = FAIL;

static void callback(
    int valid,
    unsigned char *payload,
    void *info,
    size_t size)
{
    unsigned char *valid_data = info;

    size_t i;
    printf("output: ");
    for (i = 0; i < size; i++) {
        if (payload[i] != valid_data[i])
            frame_matches = FAIL;

        printf("0x%x ", payload[i]);
    }
    printf("\n");

    frame_matches = PASS;
}

int test_ax25_framer()
{
    unsigned char data[16] = {
        0xff, 0xff, 0x1f, 0xf0,
        0xff, 0xbf, 0xef, 0x00,
        0xff, 0xff, 0xff, 0xff,
        0x00, 0x01, 0x23, 0x88
    };

    unsigned char *frame_data;
    size_t frame_size;

    pk_ax25_framer *framer = pk_ax25_framer_create(0);
    pk_ax25_deframer *deframer = pk_ax25_deframer_create(data, callback);

    pk_ax25_framer_process(framer, data, 16);
    frame_data = pk_ax25_framer_read(framer, &frame_size);
    pk_ax25_deframer_process(deframer, frame_data, frame_size);

    if (frame_matches == FAIL)
        return FAIL;

    pk_ax25_framer_destroy(framer);
    pk_ax25_deframer_destroy(deframer);

    printf("test_ax25_framer passed.\n");
    return PASS;
}

int test_ax25_framer_extra_bits()
{
    unsigned char data[16] = {
        0xff, 0xff, 0xff, 0xff,
        0xff, 0xbf, 0xff, 0x00,
        0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff
    };

    unsigned char output[8*16 + 16] = {0};
    unsigned char *frame_data;

    size_t frame_size;

    pk_ax25_framer *framer = pk_ax25_framer_create(0);
    pk_ax25_deframer *deframer = pk_ax25_deframer_create(data, callback);

    pk_ax25_framer_process(framer, data, 16);
    frame_data = pk_ax25_framer_read(framer, &frame_size);
    memcpy(output + 16, frame_data, (frame_size / 8) * sizeof(unsigned char));

    pk_ax25_deframer_process(deframer, frame_data, frame_size);

    if (frame_matches == FAIL)
        return FAIL;

    pk_ax25_framer_destroy(framer);
    pk_ax25_deframer_destroy(deframer);

    printf("test_ax25_framer_extra_bits passed.\n");
    return PASS;
}

int main(int argc, char *argv[])
{
    int result = PASS;

    // run all of the tests
    result += test_ax25_framer();
    result += test_ax25_framer_extra_bits();

    printf("all framing tests finished.\n");
    return result;
}
