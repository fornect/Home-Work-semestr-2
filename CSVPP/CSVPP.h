#pragma once

#include <stdio.h>

// Convert the CSV stream to a pseudographic table. Returns zero on success.
int writePrettyTable(FILE* input, FILE* output);

// Convert inputPath to outputPath. Returns zero on success.
int convertCsvFile(const char* inputPath, const char* outputPath);
