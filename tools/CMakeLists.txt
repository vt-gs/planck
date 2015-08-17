# Build the template parser
list(APPEND PLANCK_PARSER_SOURCES tparser.c)
add_executable(TPARSER ${PLANCK_PARSER_SOURCES})
