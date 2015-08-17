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

# Consume each templated C file
macro(expand_template SOURCE_LIST INPUT OUTPUT)
    foreach(SOURCE ${SOURCE_LIST})

        # Replace the end of file with input/output tag
        string(SUBSTRING ${INPUT} 0 1 INPUT_TAG)
        string(SUBSTRING ${OUTPUT} 0 1 OUTPUT_TAG)

        # or set the tag to something custom
        set(SUFFIX "")
        set(ARGS ${ARGN})
        list(LENGTH ARGS ARGS_LEN)
        if(${ARGS_LEN} GREATER 0)
            list(GET ARGS 0 SUFFIX)
            string(LENGTH "${SUFFIX}" SUFFIX_LEN)
            if(SUFFIX_LEN EQUAL 2)
                string(SUBSTRING ${SUFFIX} 0 1 INPUT_TAG)
                string(SUBSTRING ${SUFFIX} 1 1 OUTPUT_TAG)
            endif()
        endif()

        string(REPLACE ".t.c" ".${INPUT_TAG}${OUTPUT_TAG}.c" OUTPUT_SOURCE ${SOURCE})

        # Create a custom command for preprocessing templates
        add_custom_command(
            OUTPUT ${OUTPUT_SOURCE}
            COMMAND TPARSER "${PLANCK_SOURCE_DIR}/lib/${SOURCE}"
                    "${PLANCK_BINARY_DIR}/lib/${OUTPUT_SOURCE}" ${INPUT} ${OUTPUT} ${SUFFIX}
            DEPENDS ${SOURCE}
        )
        list(APPEND OUTPUT_SOURCES "${OUTPUT_SOURCE}")
        list(APPEND GENERATED_SOURCES "${PLANCK_BINARY_DIR}/lib/${OUTPUT_SOURCE}")
    endforeach()
    add_custom_target(exec_tparser_${OUTPUT_SOURCE}_${INPUT}_${OUTPUT} DEPENDS ${OUTPUT_SOURCES})
    list(APPEND TARGET_LIST exec_tparser_${OUTPUT_SOURCE}_${INPUT}_${OUTPUT})
endmacro()

################################################
# Source code listing and template generation  #
################################################

# Add some templated buffer, filter, matrix sources, and dot product
set(PLANCK_SOURCE_TEMPLATES
    buffers.t.c
    matrix.t.c
    dot.t.c
)

# Expand templated files into supported data types
expand_template("${PLANCK_SOURCE_TEMPLATES}" "float" "float")
expand_template("${PLANCK_SOURCE_TEMPLATES}" "float complex" "float complex" "cc")
expand_template("${PLANCK_SOURCE_TEMPLATES}" "unsigned char" "unsigned char")
expand_template("${PLANCK_SOURCE_TEMPLATES}" "int" "int")

# Filters only support real and complex coefficients
set(PLANCK_FILTER_TEMPLATE filters.t.c)

expand_template("${PLANCK_FILTER_TEMPLATE}" "float" "float")
expand_template("${PLANCK_FILTER_TEMPLATE}" "float complex" "float complex" "cc")

# Plot tooling
set(PLANCK_PLOT_TEMPLATE plot.t.c)

expand_template("${PLANCK_PLOT_TEMPLATE}" "float" "float")
expand_template("${PLANCK_PLOT_TEMPLATE}" "float complex" "float complex" "cc")
expand_template("${PLANCK_PLOT_TEMPLATE}" "int" "int")

#######################################################
# End of source code listing and template generation  #
#######################################################

# Add all the predefined library source files
list(APPEND PLANCK_SOURCES
    bits.c
    control.c
    equalization.c
    fec.c
    framers.c
    filters.c
    maths.c
    modems.c
    nco.c
    random.c
    spread.c
    sequences.c
)

# Build as a static or shared library
option(SHARED_LIB "Build as a shared library" ON)
if(SHARED_LIB)
    set(LIB_TYPE SHARED)
else()
    set(LIB_TYPE STATIC)
endif()

# Define all of the built-in library flags to include
list(APPEND PLANCK_LINKER_FLAGS
    "-Wall -lm"
)

# Build the library
set_source_files_properties(${GENERATED_SOURCES} PROPERTIES GENERATED TRUE)
add_library(
    ${PROJECT_NAME}
    ${LIB_TYPE}
    ${PLANCK_SOURCES}
    ${GENERATED_SOURCES}
)
target_link_libraries(
    ${PROJECT_NAME}
    ${KISSFFT_LIBRARIES}
    ${LIBFEC_LIBRARIES}
    ${PLANCK_LINKER_FLAGS}
)
add_dependencies(${PROJECT_NAME} "${TARGET_LIST}")

# Install the library files
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib"
    RUNTIME DESTINATION "bin"
    COMPONENT library
)
