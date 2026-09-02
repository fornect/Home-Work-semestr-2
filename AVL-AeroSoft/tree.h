#pragma once

#include <stddef.h>

#define IATA_CODE_LENGTH 3
#define IATA_CODE_CAPACITY (IATA_CODE_LENGTH + 1)
#define AIRPORT_NAME_CAPACITY 256

typedef struct Airport {
    char code[IATA_CODE_CAPACITY];
    char name[AIRPORT_NAME_CAPACITY];
} Airport;

typedef struct Node {
    Airport airport;
    int height;
    struct Node* left;
    struct Node* right;
} Node;

/* Converts a three-letter IATA code to upper case. */
int normalizeIataCode(const char* input, char output[IATA_CODE_CAPACITY]);

/* Returns 1 when inserted, 0 for a duplicate and -1 on allocation failure. */
int insertAirport(Node** root, const Airport* airport);

/* Returns 1 when deleted and 0 when the code was not found. */
int deleteAirport(Node** root, const char* code);

const Node* searchAirport(const Node* root, const char* code);
size_t countNodes(const Node* root);
int saveTreeToFile(const Node* root, const char* filename);
void freeTree(Node* root);

/* Returns the number of unique records, or -1 on any loading error. */
int loadAirports(const char* filename, Node** root);
