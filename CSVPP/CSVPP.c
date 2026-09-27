#include "CSVPP.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct Row {
    char** cells;
    size_t cellCount;
};

struct Table {
    struct Row* rows;
    size_t rowCount;
    size_t rowCapacity;
    size_t columnCount;
};

static const size_t INITIAL_CAPACITY = 16;

static void freeRow(struct Row* row)
{
    for (size_t column = 0; column < row->cellCount; ++column) {
        free(row->cells[column]);
    }
    free(row->cells);
}

static void freeTable(struct Table* table)
{
    for (size_t row = 0; row < table->rowCount; ++row) {
        freeRow(&table->rows[row]);
    }
    free(table->rows);
}

static int readLine(FILE* input, char** line, bool* reachedEnd)
{
    size_t capacity = INITIAL_CAPACITY;
    size_t length = 0;
    char* buffer = malloc(capacity);
    if (buffer == NULL) {
        return 1;
    }

    int character = fgetc(input);
    if (character == EOF) {
        free(buffer);
        *line = NULL;
        *reachedEnd = true;
        return ferror(input) != 0;
    }

    while (character != EOF && character != '\n') {
        if (length + 1 >= capacity) {
            size_t newCapacity = capacity * 2;
            char* resizedBuffer = realloc(buffer, newCapacity);
            if (resizedBuffer == NULL) {
                free(buffer);
                return 1;
            }
            buffer = resizedBuffer;
            capacity = newCapacity;
        }
        buffer[length++] = (char)character;
        character = fgetc(input);
    }

    if (character == EOF && ferror(input) != 0) {
        free(buffer);
        return 1;
    }
    if (length > 0 && buffer[length - 1] == '\r') {
        --length;
    }
    buffer[length] = '\0';
    *line = buffer;
    *reachedEnd = false;
    return 0;
}

static int copyCell(const char* start, size_t length, char** cell)
{
    char* copy = malloc(length + 1);
    if (copy == NULL) {
        return 1;
    }
    memcpy(copy, start, length);
    copy[length] = '\0';
    *cell = copy;
    return 0;
}

static int parseRow(const char* line, struct Row* row)
{
    size_t cellCount = 1;
    for (const char* character = line; *character != '\0'; ++character) {
        if (*character == ',') {
            ++cellCount;
        }
    }

    char** cells = calloc(cellCount, sizeof(*cells));
    if (cells == NULL) {
        return 1;
    }

    size_t column = 0;
    const char* cellStart = line;
    for (const char* character = line;; ++character) {
        if (*character == ',' || *character == '\0') {
            if (copyCell(cellStart, (size_t)(character - cellStart), &cells[column]) != 0) {
                struct Row partialRow = { cells, column };
                freeRow(&partialRow);
                return 1;
            }
            ++column;
            if (*character == '\0') {
                break;
            }
            cellStart = character + 1;
        }
    }

    row->cells = cells;
    row->cellCount = cellCount;
    return 0;
}

static int appendRow(struct Table* table, struct Row row)
{
    if (table->rowCount == table->rowCapacity) {
        size_t newCapacity = table->rowCapacity == 0 ? INITIAL_CAPACITY : table->rowCapacity * 2;
        struct Row* resizedRows = realloc(table->rows, newCapacity * sizeof(*resizedRows));
        if (resizedRows == NULL) {
            return 1;
        }
        table->rows = resizedRows;
        table->rowCapacity = newCapacity;
    }

    table->rows[table->rowCount++] = row;
    if (row.cellCount > table->columnCount) {
        table->columnCount = row.cellCount;
    }
    return 0;
}

static int readTable(FILE* input, struct Table* table)
{
    while (true) {
        char* line = NULL;
        bool reachedEnd = false;
        if (readLine(input, &line, &reachedEnd) != 0) {
            return 1;
        }
        if (reachedEnd) {
            return 0;
        }

        struct Row row = { 0 };
        if (parseRow(line, &row) != 0) {
            free(line);
            return 1;
        }
        free(line);
        if (appendRow(table, row) != 0) {
            freeRow(&row);
            return 1;
        }
        if (feof(input) != 0) {
            return 0;
        }
    }
}

static const char* getCell(const struct Row* row, size_t column)
{
    return column < row->cellCount ? row->cells[column] : "";
}

static size_t getColumnWidth(const struct Table* table, size_t column)
{
    size_t width = 0;
    for (size_t row = 0; row < table->rowCount; ++row) {
        size_t cellLength = strlen(getCell(&table->rows[row], column));
        if (cellLength > width) {
            width = cellLength;
        }
    }
    return width;
}

static bool isNumber(const char* value)
{
    if (*value == '+' || *value == '-') {
        ++value;
    }

    bool hasDigit = false;
    bool hasDecimalPoint = false;
    for (; *value != '\0'; ++value) {
        if (isdigit((unsigned char)*value) != 0) {
            hasDigit = true;
        } else if (*value == '.' && !hasDecimalPoint) {
            hasDecimalPoint = true;
        } else {
            return false;
        }
    }
    return hasDigit;
}

static int writeRepeated(FILE* output, char character, size_t count)
{
    for (size_t index = 0; index < count; ++index) {
        if (fputc(character, output) == EOF) {
            return 1;
        }
    }
    return 0;
}

static int writeBorder(FILE* output, const struct Table* table, char fill)
{
    for (size_t column = 0; column < table->columnCount; ++column) {
        if (fputc('+', output) == EOF
            || writeRepeated(output, fill, getColumnWidth(table, column) + 2) != 0) {
            return 1;
        }
    }
    return fputs("+\n", output) == EOF;
}

static int writeRow(FILE* output, const struct Table* table, size_t rowIndex, bool isHeader)
{
    const struct Row* row = &table->rows[rowIndex];
    if (fputc('|', output) == EOF) {
        return 1;
    }

    for (size_t column = 0; column < table->columnCount; ++column) {
        const char* cell = getCell(row, column);
        size_t cellLength = strlen(cell);
        size_t padding = getColumnWidth(table, column) - cellLength;
        bool alignRight = !isHeader && isNumber(cell);

        if (fputc(' ', output) == EOF) {
            return 1;
        }
        if (alignRight && writeRepeated(output, ' ', padding) != 0) {
            return 1;
        }
        if (fputs(cell, output) == EOF) {
            return 1;
        }
        if (!alignRight && writeRepeated(output, ' ', padding) != 0) {
            return 1;
        }
        if (fputs(" |", output) == EOF) {
            return 1;
        }
    }
    return fputc('\n', output) == EOF;
}

static int writeTable(FILE* output, const struct Table* table)
{
    if (table->rowCount == 0) {
        return 0;
    }
    if (writeBorder(output, table, '=') != 0 || writeRow(output, table, 0, true) != 0
        || writeBorder(output, table, '=') != 0) {
        return 1;
    }

    for (size_t row = 1; row < table->rowCount; ++row) {
        if (writeRow(output, table, row, false) != 0 || writeBorder(output, table, '-') != 0) {
            return 1;
        }
    }
    return 0;
}

int writePrettyTable(FILE* input, FILE* output)
{
    if (input == NULL || output == NULL) {
        return 1;
    }

    struct Table table = { 0 };
    int status = readTable(input, &table);
    if (status == 0) {
        status = writeTable(output, &table);
    }
    freeTable(&table);
    return status;
}

int convertCsvFile(const char* inputPath, const char* outputPath)
{
    FILE* input = fopen(inputPath, "r");
    if (input == NULL) {
        return 1;
    }

    FILE* output = fopen(outputPath, "w");
    if (output == NULL) {
        fclose(input);
        return 1;
    }

    int status = writePrettyTable(input, output);
    if (fclose(output) != 0) {
        status = 1;
    }
    if (fclose(input) != 0) {
        status = 1;
    }
    return status;
}
