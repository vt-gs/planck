/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Paul Uri David
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation filenames (the "Software"), to deal
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

/*
 * A simple parser to preprocess templated C files.
 * Because macros are just terrible.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_LENGTH 254

// define tags to replace
static const char output_tag[] = "<O>";
static const char input_tag[]  = "<I>";
static const char suffix_tag[] = "XX";

struct argtable_s
{
    char source[FILENAME_MAX];
    char destin[FILENAME_MAX];
    char suffix[2];

    char input_type[MAX_LENGTH];
    char output_type[MAX_LENGTH];
};

static void error_msg(const char msg[])
{
    printf("tparser error: %s\n", msg);
    exit(1);
}

// Read arguments into the structure argtable
static void arg_read(int argc, char **argv, struct argtable_s *table)
{
    if (argc < 5 || argc > 6)
        error_msg("Not enough or too many arguments");

    // Check and read type signatures
    size_t source_len = strlen(argv[1]);
    size_t destin_len = strlen(argv[2]);
    size_t output_len = strlen(argv[3]);
    size_t input_len  = strlen(argv[4]);

    assert(source_len < MAX_LENGTH);
    assert(destin_len < MAX_LENGTH);
    assert(output_len < MAX_LENGTH);
    assert(input_len  < MAX_LENGTH);

    strcpy(table->source, argv[1]);
    strcpy(table->destin, argv[2]);
    strcpy(table->input_type, argv[3]);
    strcpy(table->output_type, argv[4]);

    table->suffix[0] = table->input_type[0];
    table->suffix[1] = table->output_type[0];

    // if user provides their own suffix
    if (argc == 6) {
        size_t suffix_len = strlen(argv[5]);
        assert(suffix_len < MAX_LENGTH);
        table->suffix[0] = argv[5][0];
        table->suffix[1] = argv[5][1];
    }
}

// Match exactly a substring within a string
static int strmatch(const char *str1, const char *str2)
{
    int c, i, j;
    size_t str1len, str2len;

    str1len = strlen(str1);
    str2len = strlen(str2);

    if (str1len < str2len)
        return -1;

    for (c = 0; c <= (str1len - str2len); c++) {
        j = 0;
        for (i = 0; i < str2len; i++) {
            if (str2[i] != str1[c+i]) {
                break;
            }
            j++;
        }
        if (j == str2len)
            return c;
    }
    return -1;
}

// Find the nearest tag
static int nearest_tag(int *flag, const char *buf)
{
    int min = -1;
    int result[3];

    result[0] = strmatch(buf, suffix_tag);
    result[1] = strmatch(buf, output_tag);
    result[2] = strmatch(buf, input_tag);

    unsigned int i;
    for (i = 0; i < 3; i++) {
        if (result[i] != -1) {
            if (min > result[i] || min == -1) {
                *flag = i;
                min = result[i];
            }
        }
    }

    return min;
}

// Replace the tags within the templated file
static void replace_tags(struct argtable_s *table, const char *buf, FILE *fp)
{
    size_t i;
    size_t start = 0;
    size_t rpl_len = 0, tag_len = 0;
    size_t end = strlen(buf);

    int result = -1;
    int flag = 0;

    // Fill in the span and replace the nearest tag
    while (1) {
        result = nearest_tag(&flag, buf+start);
        if (result == -1) break;

        const char *str_ptr = NULL;

        enum {SUFFIX=0, OUTPUT, INPUT};
        switch (flag) {
            case SUFFIX:
                rpl_len = 2;
                tag_len = strlen(suffix_tag);
                str_ptr = table->suffix;
                break;

            case OUTPUT:
                rpl_len = strlen(table->output_type);
                tag_len = strlen(output_tag);
                str_ptr = table->output_type;
                break;

            case INPUT:
                rpl_len = strlen(table->input_type);
                tag_len = strlen(input_tag);
                str_ptr = table->input_type;
                break;

            default:
                error_msg("Flag should never be in this state.");
        }

        for (i = 0; i < result; i++)
            fputc(buf[i+start], fp);

        for (i = 0; i < rpl_len; i++)
            fputc(str_ptr[i], fp);

        start += result + tag_len;
    }

    // Place the rest of the line
    size_t diff = end - start;
    for (i = 0; i < diff; i++)
        fputc(buf[i+start], fp);
}

// Process the provided templated source file
static void process_file(struct argtable_s *table)
{
    FILE *output, *input;
    input = fopen(table->source, "r");

    if (input == NULL)
        error_msg("Failed to open input file.");

    output = fopen(table->destin, "w");

    if (output == NULL) {
        fclose(input);
        error_msg("Failed to open output file for writing.");
    }

    char input_line[MAX_LENGTH];
    while (1) {
        if (fgets(input_line, MAX_LENGTH, input) == NULL) break;
        replace_tags(table, input_line, output);
    }

    fclose(input);
    fclose(output);
}

int main(int argc, char **argv)
{
    struct argtable_s table;
    arg_read(argc, argv, &table);

    process_file(&table);

    printf("Finished generating %s from %s\n", table.destin, table.source);
    return 0;
}
