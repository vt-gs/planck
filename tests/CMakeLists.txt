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

# Add main unit test file
list(APPEND PLANCK_UNIT_TESTS
    test_buffers.c
    test_modems.c
    test_filters.c
    test_framers.c
    test_maths.c
    test_bits.c
    test_sequences.c
    test_random.c
)

# Add an executable
foreach(TEST ${PLANCK_UNIT_TESTS})

    # Add an executable
    string(REPLACE ".c" "" TEST_NAME ${TEST})
    add_executable(${TEST_NAME} ${TEST})
    target_link_libraries(${TEST_NAME} ${PROJECT_NAME})

    # Use CMake's CTest
    add_test(NAME ${TEST_NAME}
             COMMAND ${TEST_NAME}
    )
endforeach()
