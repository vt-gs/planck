#
# The MIT License (MIT)
#
# Copyright (c) 2015 Paul Uri David
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

# External project configuration
include(ExternalProject)

# Build KissFFT as an external project
set(KISSFFT_SOURCE_DIR "${PLANCK_SOURCE_DIR}/thirdparty/kissfft")
ExternalProject_Add(EP_KISSFFT
    PREFIX ${PLANCK_BINARY_DIR}
    DOWNLOAD_COMMAND ""
    BUILD_IN_SOURCE 1
    SOURCE_DIR ${KISSFFT_SOURCE_DIR}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND make
    INSTALL_COMMAND ""
    COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/lib
    COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/include
    COMMAND cp libkissfft.so ${CMAKE_INSTALL_PREFIX}/lib/
    COMMAND cp kiss_fft.h ${CMAKE_INSTALL_PREFIX}/include/
)

# Add the KissFFT library
add_library(KISSFFT SHARED IMPORTED)
set_property(TARGET KISSFFT PROPERTY IMPORTED_LOCATION ${CMAKE_INSTALL_PREFIX}/lib/libkissfft.so)
add_dependencies(KISSFFT EP_KISSFFT)
set(KISSFFT_LIBRARIES "${CMAKE_INSTALL_PREFIX}/lib/libkissfft.so" PARENT_SCOPE)
