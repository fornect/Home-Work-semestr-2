#include "CSVPP.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int assertConversion(const char* inputText, const char* expectedOutput)
{
    FILE* input = tmpfile();
    FILE* output = tmpfile();
    if (input == NULL || output == NULL) {
        if (input != NULL) {
            fclose(input);
        }
        if (output != NULL) {
            fclose(output);
        }
        return 1;
    }

    int failed = fputs(inputText, input) == EOF || fflush(input) != 0 || fseek(input, 0, SEEK_SET) != 0
        || writePrettyTable(input, output) != 0 || fflush(output) != 0
        || fseek(output, 0, SEEK_END) != 0;

    long outputSize = failed ? -1 : ftell(output);
    if (outputSize < 0 || fseek(output, 0, SEEK_SET) != 0) {
        failed = 1;
    }

    char* actualOutput = NULL;
    if (!failed) {
        actualOutput = malloc((size_t)outputSize + 1);
        if (actualOutput == NULL
            || fread(actualOutput, 1, (size_t)outputSize, output) != (size_t)outputSize) {
            failed = 1;
        } else {
            actualOutput[outputSize] = '\0';
            failed = strcmp(actualOutput, expectedOutput) != 0;
        }
    }

    if (failed) {
        fprintf(stderr, "Conversion mismatch.\nExpected:\n%s\nActual:\n%s\n", expectedOutput,
            actualOutput == NULL ? "<unavailable>" : actualOutput);
    }
    free(actualOutput);
    fclose(output);
    fclose(input);
    return failed;
}

static int testExample(void)
{
    return assertConversion("Test field 1,Test field 2\ntest,123\nlong string test!,28.7\nother text,3\n",
        "+===================+==============+\n"
        "| Test field 1      | Test field 2 |\n"
        "+===================+==============+\n"
        "| test              |          123 |\n"
        "+-------------------+--------------+\n"
        "| long string test! |         28.7 |\n"
        "+-------------------+--------------+\n"
        "| other text        |            3 |\n"
        "+-------------------+--------------+\n");
}

static int testEmptyCells(void)
{
    return assertConversion("A,B,C\n,12,\nx,,3.5\n", "+===+====+=====+\n"
                                                     "| A | B  | C   |\n"
                                                     "+===+====+=====+\n"
                                                     "|   | 12 |     |\n"
                                                     "+---+----+-----+\n"
                                                     "| x |    | 3.5 |\n"
                                                     "+---+----+-----+\n");
}

static int testTextThatStartsWithDigits(void)
{
    return assertConversion("Name,Value\nalpha,-12\nbeta,+3.5\ngamma,12x\n",
        "+=======+=======+\n"
        "| Name  | Value |\n"
        "+=======+=======+\n"
        "| alpha |   -12 |\n"
        "+-------+-------+\n"
        "| beta  |  +3.5 |\n"
        "+-------+-------+\n"
        "| gamma | 12x   |\n"
        "+-------+-------+\n");
}

static int testEmptyInput(void)
{
    return assertConversion("", "");
}

static int testLongCell(void)
{
    const size_t CELL_LENGTH = 2048;
    char* inputText = malloc(CELL_LENGTH + 10);
    char* expectedOutput = malloc(CELL_LENGTH * 6 + 64);
    if (inputText == NULL || expectedOutput == NULL) {
        free(inputText);
        free(expectedOutput);
        return 1;
    }

    memcpy(inputText, "Header\n", 7);
    memset(inputText + 7, 'x', CELL_LENGTH);
    inputText[CELL_LENGTH + 7] = '\n';
    inputText[CELL_LENGTH + 8] = '\0';

    char* cursor = expectedOutput;
    *cursor++ = '+';
    memset(cursor, '=', CELL_LENGTH + 2);
    cursor += CELL_LENGTH + 2;
    memcpy(cursor, "+\n| Header", 10);
    cursor += 10;
    memset(cursor, ' ', CELL_LENGTH - 6);
    cursor += CELL_LENGTH - 6;
    memcpy(cursor, " |\n+", 4);
    cursor += 4;
    memset(cursor, '=', CELL_LENGTH + 2);
    cursor += CELL_LENGTH + 2;
    memcpy(cursor, "+\n| ", 4);
    cursor += 4;
    memset(cursor, 'x', CELL_LENGTH);
    cursor += CELL_LENGTH;
    memcpy(cursor, " |\n+", 4);
    cursor += 4;
    memset(cursor, '-', CELL_LENGTH + 2);
    cursor += CELL_LENGTH + 2;
    memcpy(cursor, "+\n", 2);
    cursor += 2;
    *cursor = '\0';

    int result = assertConversion(inputText, expectedOutput);
    free(expectedOutput);
    free(inputText);
    return result;
}

int main(void)
{
    int failed = 0;
    failed += testExample();
    failed += testEmptyCells();
    failed += testTextThatStartsWithDigits();
    failed += testEmptyInput();
    failed += testLongCell();
    if (failed != 0) {
        fprintf(stderr, "%d test(s) failed.\n", failed);
        return 1;
    }
    puts("All tests passed.");
    return 0;
}
