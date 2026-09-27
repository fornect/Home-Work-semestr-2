#include "CSVPP.h"

#include <stdio.h>

int main(int argumentCount, char* arguments[])
{
    const char* inputPath = "input.csv";
    const char* outputPath = "output.txt";

    if (argumentCount == 3) {
        inputPath = arguments[1];
        outputPath = arguments[2];
    } else if (argumentCount != 1) {
        fprintf(stderr, "Usage: %s [input.csv output.txt]\n", arguments[0]);
        return 1;
    }

    if (convertCsvFile(inputPath, outputPath) != 0) {
        fprintf(stderr, "Failed to convert '%s' to '%s'.\n", inputPath, outputPath);
        return 1;
    }
    return 0;
}
