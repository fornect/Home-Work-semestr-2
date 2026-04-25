#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Airport {
    char code[5];
    char name[256];
} Airport;

typedef struct Node {
    Airport airport;
    int height;
    struct Node* left;
    struct Node* right;
} Node;

int height(Node* n);

int getBalance(Node* n);

Node* createNode(Airport airport);

Node* rotateRight(Node* a);

Node* rotateLeft(Node* a);

Node* insert(Node* node, Airport airport);

Node* minValueNode(Node* node);

Node* deleteNode(Node* root, char* code);

Node* search(Node* root, char* code);

int countNodes(Node* root);

void saveToFile(Node* root, FILE* file);

int saveTreeToFile(Node* root, const char* filename);

void freeTree(Node* root);

void trimNewline(char* str);

void toUpperCase(char* str);

int loadAirports(const char* filename, Node** root);