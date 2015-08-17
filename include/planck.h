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

#ifndef INCLUDED_PLANCK_H
#define INCLUDED_PLANCK_H

#include <string.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
typedef std::complex<float>  pk_complex;
typedef std::complex<double> pk_complex_d;
#else
#include <complex.h>
typedef float complex  pk_complex;
typedef double complex pk_complex_d;
#endif

/* Mathematical functions */
// solve for roots of a polynomial based on a Newton method by Kaj Madsen (1973).
//
// the parameter n describes the order of the polynomial aka the highest exponent.
// takes an array of coefficients (a), and writes the roots to that same array.
// in the order a[n], a[n-1], ... , a[1].
void pk_polynomial_solve_madsen(pk_complex *a, size_t n);

// ensure that the array is read from a[1] to a[n], ignoring constant a[0].
void pk_polynomial_sort_poles(pk_complex *b, int order, size_t n);

// find the closest zero to the specified pole
void pk_polynomial_sort_zeros(pk_complex *a, const pk_complex *b, size_t n);

// compute the Q function
float pk_qfunc(float num);

// compute the distance between two complex numbers
float pk_dist_cf(pk_complex a, pk_complex b);

/* Bit manipulation routines */
// pack bits into a single unsigned byte
// array [0, 0, 0, 0,  1, 1, 1, 1] -> bin 00001111 -> 0x0F
unsigned char pk_pack_byte_lr(const unsigned char *bits);

// unpack a byte into bits
// 0x0F -> bin 00001111 -> array [0, 0, 0, 0,  1, 1, 1, 1]
void pk_unpack_byte_lr(unsigned char *bits, unsigned char byte);

// pack bits into a single unsigned byte
// array [0, 0, 0, 0,  1, 1, 1, 1] -> bin 11110000 -> 0xF0
unsigned char pk_pack_byte_rl(const unsigned char *bits);

// unpack a byte into bits
// 0xF0 -> bin 11110000 -> array [0, 0, 0, 0,  1, 1, 1, 1]
void pk_unpack_byte_rl(unsigned char *bits, unsigned char byte);


/* Fast circular buffer objects for different data types */
// forward declarations
typedef struct pk_circ_ff_s pk_circ_ff;
typedef struct pk_circ_cc_s pk_circ_cc;
typedef struct pk_circ_uu_s pk_circ_uu;
typedef struct pk_circ_ii_s pk_circ_ii;

// float
// creates a circular buffer
pk_circ_ff *pk_circ_ff_create(unsigned int size);

// pushes a float onto the circular buffer
void pk_circ_ff_push(pk_circ_ff *cb, float item);

// reads the circular buffer up to some num items
void pk_circ_ff_read(pk_circ_ff *cb, float *output, size_t num);

// pops the end of the circular buffer
float pk_circ_ff_pop(pk_circ_ff *cb);

// reset the circular buffer
void pk_circ_ff_clear(pk_circ_ff *cb);

// destroys the circular buffer object
void pk_circ_ff_destroy(pk_circ_ff *cb);

// pk_complex
pk_circ_cc *pk_circ_cc_create(unsigned int size);
void pk_circ_cc_push(pk_circ_cc *cb, pk_complex item);
void pk_circ_cc_read(pk_circ_cc *cb, pk_complex *output, size_t num);
pk_complex pk_circ_cc_pop(pk_circ_cc *cb);
void pk_circ_cc_clear(pk_circ_cc *cb);
void pk_circ_cc_destroy(pk_circ_cc *cb);

// unsigned characters
pk_circ_uu *pk_circ_uu_create(unsigned int size);
void pk_circ_uu_push(pk_circ_uu *cb, unsigned char item);
void pk_circ_uu_read(pk_circ_uu *cb, unsigned char *output, size_t num);
unsigned char pk_circ_uu_pop(pk_circ_uu *cb);
void pk_circ_uu_clear(pk_circ_uu *cb);
void pk_circ_uu_destroy(pk_circ_uu *cb);

// integers
pk_circ_ii *pk_circ_ii_create(unsigned int size);
void pk_circ_ii_push(pk_circ_ii *cb, int item);
void pk_circ_ii_read(pk_circ_ii *cb, int *output, size_t num);
int pk_circ_ii_pop(pk_circ_ii *cb);
void pk_circ_ii_clear(pk_circ_ii *cb);
void pk_circ_ii_destroy(pk_circ_ii *cb);

/* Resizable block of data */
// forward declarations of the different block types
typedef struct pk_block_ff_s pk_block_ff;
typedef struct pk_block_cc_s pk_block_cc;
typedef struct pk_block_uu_s pk_block_uu;
typedef struct pk_block_ii_s pk_block_ii;

// floats
// create a block of data
pk_block_ff *pk_block_ff_create(size_t size);

// resize a block of data
void pk_block_ff_resize(pk_block_ff *b, size_t new_size);

// return a pointer to the block of data
float *pk_block_ff_ptr(pk_block_ff *b);

// push an item onto the block
void pk_block_ff_push(pk_block_ff *b, float item);

// return the number of items in this pk_block
size_t pk_block_ff_nitems(pk_block_ff *b);

// return the current size
size_t pk_block_ff_size(pk_block_ff *b);

// reset block nitem counter
void pk_block_ff_clear(pk_block_ff *b);

// destroy the block of data
void pk_block_ff_destroy(pk_block_ff *b);

// pk_complex
pk_block_cc *pk_block_cc_create(size_t size);
void pk_block_cc_resize(pk_block_cc *b, size_t new_size);
pk_complex *pk_block_cc_ptr(pk_block_cc *b);
void pk_block_cc_push(pk_block_cc *b, pk_complex item);
size_t pk_block_cc_nitems(pk_block_cc *b);
size_t pk_block_cc_size(pk_block_cc *b);
void pk_block_cc_clear(pk_block_cc *b);
void pk_block_cc_destroy(pk_block_cc *b);

// unsigned char
pk_block_uu *pk_block_uu_create(size_t size);
void pk_block_uu_resize(pk_block_uu *b, size_t new_size);
unsigned char *pk_block_uu_ptr(pk_block_uu *b);
void pk_block_uu_push(pk_block_uu *b, unsigned char item);
size_t pk_block_uu_nitems(pk_block_uu *b);
size_t pk_block_uu_size(pk_block_uu *b);
void pk_block_uu_clear(pk_block_uu *b);
void pk_block_uu_destroy(pk_block_uu *b);

// integers
pk_block_ii *pk_block_ii_create(size_t size);
void pk_block_ii_resize(pk_block_ii *b, size_t new_size);
int *pk_block_ii_ptr(pk_block_ii *b);
void pk_block_ii_push(pk_block_ii *b, int item);
size_t pk_block_ii_nitems(pk_block_ii *b);
size_t pk_block_ii_size(pk_block_ii *b);
void pk_block_ii_clear(pk_block_ii *b);
void pk_block_ii_destroy(pk_block_ii *b);


/* Simple FIFO queue based on a linked list */
// forward declarations of queue objects
typedef struct pk_queue_ff_s pk_queue_ff;
typedef struct pk_queue_cc_s pk_queue_cc;
typedef struct pk_queue_uu_s pk_queue_uu;
typedef struct pk_queue_ii_s pk_queue_ii;

// float
// creates a FIFO queue that utilizes a linked list
pk_queue_ff *pk_queue_ff_create();

// insert an item into the queue
void pk_queue_ff_insert(pk_queue_ff *q, float item);

// dequeue a single item at the beginning of the list
void pk_queue_ff_dequeue(pk_queue_ff *q);

// dequeue the all elements
void pk_queue_ff_clear(pk_queue_ff *q);

// return the number of items
size_t pk_queue_ff_nitems(pk_queue_ff *q);

// read some number of items from the queue
// starting from the beginning of the queue
void pk_queue_ff_read(pk_queue_ff *q, float *output, size_t num);

// destroy the queue object
void pk_queue_ff_destroy(pk_queue_ff *q);

// pk_complex
pk_queue_cc *pk_queue_cc_create();
void pk_queue_cc_insert(pk_queue_cc *q, pk_complex item);
void pk_queue_cc_dequeue(pk_queue_cc *q);
void pk_queue_cc_clear(pk_queue_cc *q);
size_t pk_queue_cc_nitems(pk_queue_cc *q);
void pk_queue_cc_read(pk_queue_cc *q, pk_complex *output, size_t num);
void pk_queue_cc_destroy(pk_queue_cc *q);

// unsigned char
pk_queue_uu *pk_queue_uu_create();
void pk_queue_uu_insert(pk_queue_uu *q, unsigned char item);
void pk_queue_uu_dequeue(pk_queue_uu *q);
void pk_queue_uu_clear(pk_queue_uu *q);
size_t pk_queue_uu_nitems(pk_queue_uu *q);
void pk_queue_uu_read(pk_queue_uu *q, unsigned char *output, size_t num);
void pk_queue_uu_destroy(pk_queue_uu *q);

// integer
pk_queue_ii *pk_queue_ii_create();
void pk_queue_ii_insert(pk_queue_ii *q, int item);
void pk_queue_ii_dequeue(pk_queue_ii *q);
void pk_queue_ii_clear(pk_queue_ii *q);
size_t pk_queue_ii_nitems(pk_queue_ii *q);
void pk_queue_ii_read(pk_queue_ii *q, int *output, size_t num);
void pk_queue_ii_destroy(pk_queue_ii *q);


/* Dot product object */
// forward declarations of dot product objects
typedef struct pk_dotprod_ff_s pk_dotprod_ff;
typedef struct pk_dotprod_ii_s pk_dotprod_ii;
typedef struct pk_dotprod_uu_s pk_dotprod_uu;
typedef struct pk_dotprod_cc_s pk_dotprod_cc;

// float
// creates a dot product object that holds a sequence of coefficients
pk_dotprod_ff *pk_dotprod_ff_create(const float *seq, size_t size);

// executes a dot product against an incoming sequence of coefficients
float pk_dotprod_ff_execute(pk_dotprod_ff *dp, const float *input, size_t size);

// destroys a dot product object
void pk_dotprod_ff_destroy(pk_dotprod_ff *dp);

pk_dotprod_ii *pk_dotprod_ii_create(const int *seq, size_t size);
int pk_dotprod_ii_execute(pk_dotprod_ii *dp, const int *input, size_t size);
void pk_dotprod_ii_destroy(pk_dotprod_ii *dp);

pk_dotprod_uu *pk_dotprod_uu_create(const unsigned char *seq, size_t size);
unsigned char pk_dotprod_uu_execute(pk_dotprod_uu *dp, const unsigned char *input, size_t size);
void pk_dotprod_uu_destroy(pk_dotprod_uu *dp);

pk_dotprod_cc *pk_dotprod_cc_create(const pk_complex *seq, size_t size);
pk_complex pk_dotprod_cc_execute(pk_dotprod_cc *dp, const pk_complex *input, size_t size);
void pk_dotprod_cc_destroy(pk_dotprod_cc *dp);


/* Modems */
/* Binary FSK modem meant for AFSK 1200 */
// forward declarations declarations of the FSK modem
typedef struct pk_bfskmod_s pk_bfskmod;
typedef struct pk_bfskdemod_s pk_bfskdemod;

// create an FSK modulator
pk_bfskmod *pk_bfskmod_create(
    unsigned int samp_sym,   // samples per symbol
    unsigned int baud,       // baud rate
    float mark_freq,         // mark frequency  (1)
    float space_fre          // space frequency (0)
);

// execute on a bit
void pk_bfskmod_execute(pk_bfskmod *fm, pk_complex *sym, unsigned char bit);

// process a batch of bits
void pk_bfskmod_process(
    pk_bfskmod *fm,
    pk_complex *output,
    const unsigned char *input,
    size_t num
);

// destroy the FSK modulator
void pk_bfskmod_destroy(pk_bfskmod *fm);

// create an FSK demodulator
pk_bfskdemod *pk_bfskdemod_create(
    unsigned int samp_sym,   // samples per symbol
    unsigned int baud,       // baud rate
    float mark_freq,         // mark frequency  (1)
    float space_freq         // space frequency (0)
);

// execute on a symbol
unsigned char pk_bfskdemod_execute(pk_bfskdemod *fd, const pk_complex *samples);

// process a batch of samples
// warning: clears output buffer upon execution
void pk_bfskdemod_process(
    pk_bfskdemod *fd,
    const pk_complex *input,
    size_t num
);

// return a pointer to the output block of data
// and reads out the number of bits decoded
unsigned char *pk_bfskdemod_read(pk_bfskdemod *fd, size_t *nitems);

// destroy the FSK demodulator
void pk_bfskdemod_destroy(pk_bfskdemod *fd);


/* Quadrature modulator */
// TODO
// forward declarations for the quadrature modulator


/* Quadrature demodulator */
// TODO
// forward declarations for the quadrature demodulator


/* Staggered Quadrature Phase-shift Keying (OQPSK) modulator */
// TODO
// forward declarations for the OQPSK modulator


/* Staggered Quadrature Phase-shift Keying (OQPSK) demodulator */
// TODO
// forward declarations for the OQPSK demodulator


/* Gaussian Minimum-shift Keying (GMSK) modulator */
// TODO
// forward declarations for the GMSK modulator


/* Gaussian Minimum-shift Keying (GMSK) demodulator */
// TODO
// forward declarations for the GMSK demodulator


/* Filter objects */
// FIR structure
//
// forward declarations of the FIR filter
typedef struct pk_fir_ff_s pk_fir_ff;
typedef struct pk_fir_cc_s pk_fir_cc;

// float
// create a FIR filter structure
pk_fir_ff *pk_fir_ff_create(unsigned int order, const float *coeff);

// load the FIR structure with new coefficients
void pk_fir_ff_load(pk_fir_ff *fir, const float *coeff);

// execute the FIR filter over some set of samples
void pk_fir_ff_execute(pk_fir_ff *fir, float *output, const float *samples, size_t size);

// destroy the FIR filter object
void pk_fir_ff_destroy(pk_fir_ff *fir);

// pk_complex
pk_fir_cc *pk_fir_cc_create(unsigned int order, const pk_complex *coeff);
void pk_fir_cc_load(pk_fir_cc *fir, const pk_complex *coeff);
void pk_fir_cc_execute(pk_fir_cc *fir, pk_complex *output, const pk_complex *samples, size_t size);
void pk_fir_cc_destroy(pk_fir_cc *fir);


/* Second-order IIR alternate direct form I */
// forward declarations of the second-order IIR filter
typedef struct pk_iirso_ff_s pk_iirso_ff;
typedef struct pk_iirso_cc_s pk_iirso_cc;

// float
// creates a second-order IIR filter
pk_iirso_ff *pk_iirso_ff_create(const float *a, const float *b);

// load the IIR with new coefficients
void pk_iirso_ff_load(pk_iirso_ff *iir, const float *a, const float *b);

// execute the IIR filter over some set of samples
void pk_iirso_ff_execute(pk_iirso_ff *iir, float *output, const float *samples, size_t size);

// destroy the IIR filter object
void pk_iirso_ff_destroy(pk_iirso_ff *iir);

// pk_complex
pk_iirso_cc *pk_iirso_cc_create(const pk_complex *a, const pk_complex *b);
void pk_iirso_cc_load(pk_iirso_cc *iir, const pk_complex *a, const pk_complex *b);
void pk_iirso_cc_execute(pk_iirso_cc *iir, pk_complex *output, const pk_complex *samples, size_t size);
void pk_iirso_cc_destroy(pk_iirso_cc *iir);


/* Partitioned general order IIR */
// forward declarations of the IIR filter
typedef struct pk_iir_cascade_s pk_iir_cascade;

// creates a general order IIR filter
pk_iir_cascade *pk_iir_cascade_create(
    unsigned int order,     // order of the IIR
    unsigned int cascade,   // Sort by closest (0) to furthest (1) to unit circle
    const pk_complex *a, // numerator coefficients
    const pk_complex *b  // denominator coefficients
);

// load the IIR with new coefficients and re-partitions
void pk_iir_cascade_load(pk_iir_cascade *iir, const pk_complex *a, const pk_complex *b);

// execute the IIR filter over some set of samples
void pk_iir_cascade_execute(pk_iir_cascade *iir, pk_complex *output, const pk_complex *samples, size_t size);

// destroy the IIR filter object
void pk_iir_cascade_destroy(pk_iir_cascade *iir);


/* General order direct form IIR filter */
// forward declarations
// TODO


/* Pulse shaping filters */
// Gaussian pulse shaping
void pk_firdes_gaussian(
    float *taps,
    float bt,
    float delay,
    unsigned int span
);

// TODO
void pk_firdes_gaussian_noisi(
    float *taps,
    float bt,
    float delay,
    unsigned int span
);

// TODO
void pk_firdes_rrc(
    float *taps,
    float beta,
    float delay,
    unsigned int span
);

// TODO
void pk_firdes_srrc(
    float *taps,
    float beta,
    float delay,
    unsigned int span
);

/* fec and error detection objects */
// algorithm comes from Kenneth W. Finnegan's APRS thesis.
// this particular CRC function is appropriate for AX.25
unsigned int crc_ax25_byte(const unsigned char *data, size_t size);


/* Framer and deframer objects */
// forward declaration for the AX.25 framer/deframer objects
typedef struct pk_ax25_framer_s pk_ax25_framer;
typedef struct pk_ax25_deframer_s pk_ax25_deframer;

/* AX.25 framer */
// create an AX.25 framer object
// padding specifies how many zero bits to prepend and append
pk_ax25_framer *pk_ax25_framer_create(unsigned int padding);

// execute the framer on a sequence of packed bytes
void pk_ax25_framer_process(
    pk_ax25_framer *f,
    const unsigned char *bytes,
    size_t size
);

// read the output of the framer
// Also provides an optional count of the number of items
unsigned char *pk_ax25_framer_read(pk_ax25_framer *f, size_t *out_nitems);

// destroy the AX.25 framer object
void pk_ax25_framer_destroy(pk_ax25_framer *f);

/* AX.25 deframer */
// create an AX.25 deframer object and
// provide a callback function for frames
// note that the info pointer can be used for anything
pk_ax25_deframer *pk_ax25_deframer_create(
    void *info,
    void (*callback_ptr)(int valid, unsigned char *payload, void *info, size_t size)
);

// execute a deframer on a sequence of bits
void pk_ax25_deframer_process(
    pk_ax25_deframer *df,
    const unsigned char *bits,
    size_t size
);

// destroy the AX.25 deframer
void pk_ax25_deframer_destroy(pk_ax25_deframer *df);


/* Sequence generators and objects */
/* m-sequence object */
// forward declarations for the lfsr object
typedef struct pk_lfsr_s pk_lfsr;

// create the lfsr object with some number
// of bits and start state
pk_lfsr *pk_lfsr_create(unsigned int n, uint32_t start);

// execute the lfsr and optionally take where it is in its period
// returns the state of the lfsr
uint32_t pk_lfsr_execute(pk_lfsr *l, int *out_period);

// destroy the lfsr object
void pk_lfsr_destroy(pk_lfsr *l);


/* Scrambler objects */
// forward declarations for scrambler/descrambler objects
typedef struct pk_add_scrambler_s pk_add_scrambler;
typedef struct pk_mult_scrambler_s pk_mult_scrambler;
typedef struct pk_mult_descrambler_s pk_mult_descrambler;

/* Additive scrambler/descrambler object */
// create the additive scrambler
// note that the scrambler also serves as a descrambler
pk_add_scrambler *pk_add_scrambler_create(unsigned int n, uint32_t start);

// insert some input and return an output bit from the scrambler
unsigned char pk_add_scrambler_execute(pk_add_scrambler *as, unsigned char input);

// destroy the scrambler object
void pk_add_scrambler_destroy(pk_add_scrambler *as);


/* Multiplicative scrambler */
// create the multiplicative scrambler
pk_mult_scrambler *pk_mult_scrambler_create(unsigned int n, uint32_t start);

// insert some input and return an output bit from the scrambler
unsigned char pk_mult_scrambler_execute(pk_mult_scrambler *ms, unsigned char input);

// destroy the scrambler object
void pk_mult_scrambler_destroy(pk_mult_scrambler *ms);


/* Multiplicative descrambler */
// create a multiplicative descrambler
pk_mult_descrambler *pk_mult_descrambler_create(unsigned int n, uint32_t start);

// insert a scrambled bit and return the next unscrambled bit
unsigned char pk_mult_descrambler_execute(pk_mult_descrambler *md, unsigned char input);

// destroy the descrambler object
void pk_mult_descrambler_destroy(pk_mult_descrambler *md);

#endif

#ifdef __cplusplus
}
#endif
