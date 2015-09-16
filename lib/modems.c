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

/* BFSK modem objects */
// continuous binary BFSK modulator meant
// for amateur radio modems like AFSK 1200
// Uses differential coding and non-coherent demodulation
typedef struct pk_bfskmod_s
{
    unsigned int samp_sym;
    unsigned char past;
    float samp_rate;

    float phase;
    float phase_inc;

    float mark_freq;
    float space_freq;
} pk_bfskmod;

// create an BFSK modulator
pk_bfskmod *pk_bfskmod_create(
    unsigned int samp_sym,   // samples per symbol
    unsigned int baud,       // baud rate
    float mark_freq,         // mark frequency  (1)
    float space_freq)        // space frequency (0)
{
    pk_bfskmod *fm = malloc(sizeof(pk_bfskmod));
    fm->samp_sym = samp_sym;
    fm->samp_rate = fm->samp_sym * baud;

    fm->phase_inc = 0;
    fm->phase = 0;
    fm->past = 0;

    fm->mark_freq = (2.0f * M_PI * mark_freq / fm->samp_rate);
    fm->space_freq = (2.0f * M_PI * space_freq / fm->samp_rate);

    return fm;
}

// execute on a bit and produce a number of samples per symbol
void pk_bfskmod_execute(pk_bfskmod *fm, float complex *sym, unsigned char bit)
{
    if (bit == 0) fm->past = fm->past != 1;
    fm->phase_inc = fm->past ? fm->mark_freq : fm->space_freq;

    size_t i;
    for (i = 0; i < fm->samp_sym; i++) {
        fm->phase += fm->phase_inc;

        // perform phase unwrapping
        while (fm->phase > 2*M_PI)
            fm->phase -= 2*M_PI;

        sym[i] = cexpf(I * fm->phase);
    }
}

// process a batch of bits
void pk_bfskmod_process(
    pk_bfskmod *fm,
    float complex *output,
    const unsigned char *input,
    size_t num)
{
    size_t i;
    for (i = 0; i < num; i++)
        pk_bfskmod_execute(fm, &output[i*fm->samp_sym], input[i]);
}

// destroy the BFSK modulator
void pk_bfskmod_destroy(pk_bfskmod *fm)
{
    free(fm);
}

// continuous BFSK demodulator
typedef struct pk_bfskdemod_s
{
    unsigned int samp_sym;

    unsigned int diff;
    unsigned int timer;
    unsigned char past;

    float complex *mark_filt;
    float complex *space_filt;

    pk_circ_cc *window;
    pk_block_uu *data;
} pk_bfskdemod;

// create an binary BFSK demodulator
pk_bfskdemod *pk_bfskdemod_create(
    unsigned int samp_sym,   // samples per symbol
    unsigned int baud,       // baud rate
    float mark_freq,         // mark frequency  (1)
    float space_freq)        // space frequency (0)
{
    pk_bfskdemod *fd  = malloc(sizeof(pk_bfskdemod));
    fd->samp_sym = samp_sym;

    fd->data = pk_block_uu_create(1024);
    fd->mark_filt  = malloc(fd->samp_sym * sizeof(float complex));
    fd->space_filt = malloc(fd->samp_sym * sizeof(float complex));

    fd->diff = 0;
    fd->timer = 0;
    fd->past = 0;

    float samp_rate = fd->samp_sym * baud;
    float mphase_inc = (2.0f * M_PI *  mark_freq / samp_rate);
    float sphase_inc = (2.0f * M_PI * space_freq / samp_rate);

    float mphase = 0;
    float sphase = 0;

    // Create the matched filters
    size_t i;
    for (i = 0; i < fd->samp_sym; i++) {
        mphase += mphase_inc;
        sphase += sphase_inc;

        while (mphase > 2*M_PI)
            mphase -= 2*M_PI;

        while (sphase > 2*M_PI)
            sphase -= 2*M_PI;

        fd->mark_filt[i]  = cexpf(-1.0f * I * mphase);
        fd->space_filt[i] = cexpf(-1.0f * I * sphase);
    }

    // create window
    fd->window = pk_circ_cc_create(fd->samp_sym);

    return fd;
}

// execute on a symbol using matched filters
unsigned char pk_bfskdemod_execute(pk_bfskdemod *fd, const float complex *samples)
{
    float complex mark = 0;
    float complex space = 0;

    size_t i;
    for (i = 0; i < fd->samp_sym; i++) {
        mark  += (samples[i] * fd->mark_filt[i]);
        space += (samples[i] * fd->space_filt[i]);
    }

    float m = cabsf(mark);
    float s = cabsf(space);

    return m > s;
}

// process a batch of samples
void pk_bfskdemod_process(
    pk_bfskdemod *fd,
    const float complex *input,
    size_t num)
{
    pk_block_uu_clear(fd->data);

    size_t i;
    for (i = 0; i < num; i++) {
        fd->timer++;

        float complex wb[fd->samp_sym];
        unsigned char match_filt;

        pk_circ_cc_push(fd->window, input[i]);
        pk_circ_cc_read(fd->window, wb, fd->samp_sym);

        match_filt = pk_bfskdemod_execute(fd, wb);

        // adjust our timing based on the NRZI transitions
        if (match_filt != fd->past) {
            fd->diff = 1;
            fd->past = match_filt;
            fd->timer = fd->samp_sym / 2 + fd->samp_sym + 1;
        }

        // make a bit decision and push it
        if (fd->timer >= 2*fd->samp_sym) {
            unsigned char bit = fd->diff == 0;
            pk_block_uu_push(fd->data, bit);
            fd->timer = fd->samp_sym;
            fd->diff = 0;
        }
    }
}

// return a pointer to the output block of data
unsigned char *pk_bfskdemod_read(pk_bfskdemod *fd, size_t *nitems)
{
    *nitems = pk_block_uu_nitems(fd->data);
    return pk_block_uu_ptr(fd->data);
}

// destroy the BFSK demodulator
void pk_bfskdemod_destroy(pk_bfskdemod *fd)
{
    pk_circ_cc_destroy(fd->window);
    pk_block_uu_destroy(fd->data);

    free(fd->mark_filt);
    free(fd->space_filt);

    free(fd);
}


/* FSK96 modem objects */
typedef struct pk_fsk96mod_s
{
    unsigned int samp_sym;
    unsigned char past;

} pk_fsk96mod;

pk_fsk96mod *pk_fsk96mod_create(
    unsigned int samp_sym)
{
    pk_fsk96mod *fm = malloc(sizeof(pk_fsk96mod));
    fm->samp_sym = samp_sym;

    fm->past = 0;

    return fm;
}

// execute on a bit and produce a number of samples per symbol
void pk_fsk96mod_execute(pk_fsk96mod *fm, float *sym, unsigned char bit)
{
    if (bit == 0) fm->past = fm->past != 1;
    float symbol = (float) (2* fm->past - 1) / fm->samp_sym;

    sym[0] = symbol;

    size_t i;
    for (i = 1; i < fm->samp_sym; i++)
        sym[i] = symbol;
}

// process a batch of bits
void pk_fsk96mod_process(
    pk_fsk96mod *fm,
    float *output,
    const unsigned char *input,
    size_t num)
{
    size_t i;
    for (i = 0; i < num; i++)
        pk_fsk96mod_execute(fm, &output[i*fm->samp_sym], input[i]);
}

// destroy the FSK96 modulator
void pk_fsk96mod_destroy(pk_fsk96mod *fm)
{
    free(fm);
}

typedef struct pk_fsk96demod_s
{
    unsigned int samp_sym;

    unsigned int diff;
    unsigned int timer;
    unsigned char past;

    pk_circ_ff *window;
    pk_block_uu *data;
} pk_fsk96demod;

pk_fsk96demod *pk_fsk96demod_create(unsigned int samp_sym)
{
    pk_fsk96demod *fd = malloc(sizeof(pk_fsk96demod));
    fd->samp_sym = samp_sym;

    fd->data = pk_block_uu_create(1024);

    fd->diff = 0;
    fd->timer = 0;
    fd->past = 0;

    fd->window = pk_circ_ff_create(fd->samp_sym);

    return fd;
}

// execute on a symbol using integrate & dump filter
// TODO: Improve this
unsigned char pk_fsk96demod_execute(pk_fsk96demod *fd, const float *samples)
{
    float result = 0;

    size_t i;
    for (i = 0; i < fd->samp_sym; i++)
       result += samples[i];

    return result > 0;
}

// process a batch of samples
void pk_fsk96demod_process(
    pk_fsk96demod *fd,
    const float *samples,
    size_t num)
{
    pk_block_uu_clear(fd->data);

    size_t i;
    for (i = 0; i < num; i++) {
        fd->timer++;

        float wb[fd->samp_sym];
        unsigned char match_filt;

        pk_circ_ff_push(fd->window, samples[i]);
        pk_circ_ff_read(fd->window, wb, fd->samp_sym);

        match_filt = pk_fsk96demod_execute(fd, wb);

        // adjust our timing
        if (match_filt != fd->past) {
            fd->diff = 1;
            fd->past = match_filt;
            fd->timer = fd->samp_sym / 2 + fd->samp_sym + 1;
        }

        // make a bit decision and push it
        if (fd->timer >= 2*fd->samp_sym) {
            unsigned char bit = fd->diff == 0;
            pk_block_uu_push(fd->data, bit);
            fd->timer = fd->samp_sym;
            fd->diff = 0;
        }
    }
}

// return a pointer to the output block of data
unsigned char *pk_fsk96demod_read(pk_fsk96demod *fd, size_t *nitems)
{
    *nitems = pk_block_uu_nitems(fd->data);
    return pk_block_uu_ptr(fd->data);
}

void pk_fsk96demod_destroy(pk_fsk96demod *fd)
{
    pk_circ_ff_destroy(fd->window);
    pk_block_uu_destroy(fd->data);

    free(fd);
}
