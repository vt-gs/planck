planck-dsp
==========

A minimalist and portable DSP library for wireless communications written in C.
This library opts for keeping things as simple as possible, making it easy to bind
to scripting languages. This was created to support many other projects.

Please note that this library is a work-in-progress.

Features
--------

In addition to a KISS library structure, this library includes:

* A variety of digital and analog modems.
* Important communication tools like AGCs and a myriad of synchronization control loops like PLLs.
* Unique support for spread spectrum techniques.
* OFDM and other multichannel waveforms.
* Support for common transforms and digital signal processing routines.
* Important filtering infrastructure like FIR, IIR, and FFT-based filters.
* Necessary support for framing, sequences, bit manipulation, stochastics and matrix mathematics.
* Forward-error correcting encoding and decoding.
* Special support for amateur radio modes.

And finally, in order to avoid issues with foreign function interfaces of certain
languages, a parser was included to generate C code from simple templates.

Dependencies
------------

We aim to reduce the number of dependencies, but some are unavoidable.

* CMake
* KissFFT

Install
-------

Follow these steps. Make sure to run `make test` to ensure the software is validated.

    $ git clone (the github/bitbucket address)
    $ cd planck
    $ mkdir build && cd build
    $ cmake ../ -DCMAKE_INSTALL_PREFIX=$HOME/install/target
    $ make test
    $ make install

Benchmarks and Examples
-----------------------

Ensure the library validates all software tests, and run either

    $ make benchmarks

or

    $ make examples
