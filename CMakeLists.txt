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

# Project configuration
cmake_minimum_required(VERSION 2.8)
project(planck CXX C)
enable_testing()

set(PLANCK_VERSION 0.0.0)

set(PLANCK_SOURCE_DIR "${CMAKE_SOURCE_DIR}")
set(PLANCK_BINARY_DIR "${CMAKE_BINARY_DIR}")
set(PLANCK_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Set cmake modules / toolchains / etc.
set(CMAKE_CONFIG_PATH "${PLANCK_SOURCE_DIR}/cmake")
set(CMAKE_MODULE_PATH "${PLANCK_SOURCE_DIR}/cmake/modules")
set(CMAKE_TOOLCHAIN_PATH "${PLANCK_SOURCE_DIR}/cmake/toolchains")
set(CMAKE_MACRO_PATH "${PLANCK_SOURCE_DIR}/cmake/macros")

# Add include directories
include_directories(${PLANCK_INCLUDE_DIR})
include_directories(${CMAKE_INSTALL_PREFIX}/include)

# Add library subdirectories
add_subdirectory(thirdparty)
add_subdirectory(tools)
add_subdirectory(include)
add_subdirectory(lib)
add_subdirectory(tests)

# Add uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY
)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)

# Install cmake configuration files
include(${CMAKE_CONFIG_PATH}/planckConfig.cmake)
configure_file(
    ${CMAKE_CONFIG_PATH}/planckVersion.cmake.in
    ${PLANCK_BINARY_DIR}/planckVersion.cmake
    @ONLY
)

install(FILES
    "${CMAKE_CONFIG_PATH}/planckConfig.cmake"
    "${PLANCK_BINARY_DIR}/planckVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_DIR}
)
