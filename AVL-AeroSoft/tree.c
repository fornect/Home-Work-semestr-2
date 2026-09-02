#include "tree.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int nodeHeight(const Node* node)
{
    return node ? node->height : 0;
}

static int maxInt(int first, int second)
{
    return first > second ? first : second;
}

static void updateHeight(Node* node)
{
    node->height = 1 + maxInt(nodeHeight(node->left), nodeHeight(node->right));
}

static int balanceFactor(const Node* node)
{
    return node ? nodeHeight(node->left) - nodeHeight(node->right) : 0;
}

static Node* createNode(const Airport* airport)
{
    Node* node = malloc(sizeof(*node));
    if (!node) {
        return NULL;
    }

    node->airport = *airport;
    node->height = 1;
    node->left = NULL;
    node->right = NULL;
    return node;
}

static Node* rotateRight(Node* root)
{
    if (!root || !root->left) {
        return root;
    }

    Node* newRoot = root->left;
    Node* transferredSubtree = newRoot->right;

    newRoot->right = root;
    root->left = transferredSubtree;

    updateHeight(root);
    updateHeight(newRoot);
    return newRoot;
}

static Node* rotateLeft(Node* root)
{
    if (!root || !root->right) {
        return root;
    }

    Node* newRoot = root->right;
    Node* transferredSubtree = newRoot->left;

    newRoot->left = root;
    root->right = transferredSubtree;

    updateHeight(root);
    updateHeight(newRoot);
    return newRoot;
}

static Node* rebalance(Node* root)
{
    updateHeight(root);
    int balance = balanceFactor(root);

    if (balance > 1) {
        if (balanceFactor(root->left) < 0) {
            root->left = rotateLeft(root->left);
        }
        return rotateRight(root);
    }

    if (balance < -1) {
        if (balanceFactor(root->right) > 0) {
            root->right = rotateRight(root->right);
        }
        return rotateLeft(root);
    }

    return root;
}

int normalizeIataCode(const char* input, char output[IATA_CODE_CAPACITY])
{
    if (!input || !output || strlen(input) != IATA_CODE_LENGTH) {
        return 0;
    }

    for (size_t i = 0; i < IATA_CODE_LENGTH; ++i) {
        unsigned char character = (unsigned char)input[i];
        character = (unsigned char)toupper(character);
        if (character < 'A' || character > 'Z') {
            return 0;
        }
        output[i] = (char)character;
    }
    output[IATA_CODE_LENGTH] = '\0';
    return 1;
}

static Node* insertRecursive(Node* root, const Airport* airport, int* result)
{
    if (!root) {
        Node* node = createNode(airport);
        *result = node ? 1 : -1;
        return node;
    }

    int comparison = strcmp(airport->code, root->airport.code);
    if (comparison == 0) {
        *result = 0;
        return root;
    }

    if (comparison < 0) {
        Node* left = insertRecursive(root->left, airport, result);
        if (*result < 0) {
            return root;
        }
        root->left = left;
    } else {
        Node* right = insertRecursive(root->right, airport, result);
        if (*result < 0) {
            return root;
        }
        root->right = right;
    }

    return rebalance(root);
}

int insertAirport(Node** root, const Airport* airport)
{
    if (!root || !airport) {
        return -1;
    }

    int result = 0;
    *root = insertRecursive(*root, airport, &result);
    return result;
}

static const Node* minimumNode(const Node* root)
{
    const Node* current = root;
    while (current->left) {
        current = current->left;
    }
    return current;
}

static Node* deleteRecursive(Node* root, const char* code, int* deleted)
{
    if (!root) {
        return NULL;
    }

    int comparison = strcmp(code, root->airport.code);
    if (comparison < 0) {
        root->left = deleteRecursive(root->left, code, deleted);
    } else if (comparison > 0) {
        root->right = deleteRecursive(root->right, code, deleted);
    } else {
        *deleted = 1;
        if (!root->left || !root->right) {
            Node* child = root->left ? root->left : root->right;
            free(root);
            return child;
        }

        const Node* successor = minimumNode(root->right);
        root->airport = successor->airport;
        root->right = deleteRecursive(root->right, successor->airport.code, deleted);
    }

    return rebalance(root);
}

int deleteAirport(Node** root, const char* code)
{
    if (!root || !code) {
        return 0;
    }

    int deleted = 0;
    *root = deleteRecursive(*root, code, &deleted);
    return deleted;
}

const Node* searchAirport(const Node* root, const char* code)
{
    while (root) {
        int comparison = strcmp(code, root->airport.code);
        if (comparison == 0) {
            return root;
        }
        root = comparison < 0 ? root->left : root->right;
    }
    return NULL;
}

size_t countNodes(const Node* root)
{
    if (!root) {
        return 0;
    }
    return 1 + countNodes(root->left) + countNodes(root->right);
}

static int writeTree(const Node* root, FILE* file)
{
    if (!root) {
        return 1;
    }

    return writeTree(root->left, file)
        && fprintf(file, "%s:%s\n", root->airport.code, root->airport.name) >= 0
        && writeTree(root->right, file);
}

int saveTreeToFile(const Node* root, const char* filename)
{
    if (!filename) {
        return 0;
    }

    size_t temporaryLength = strlen(filename) + sizeof(".tmp");
    char* temporaryFilename = malloc(temporaryLength);
    if (!temporaryFilename) {
        return 0;
    }
    snprintf(temporaryFilename, temporaryLength, "%s.tmp", filename);

    FILE* file = fopen(temporaryFilename, "w");
    if (!file) {
        free(temporaryFilename);
        return 0;
    }

    int success = writeTree(root, file);
    if (success && fflush(file) != 0) {
        success = 0;
    }
    if (fclose(file) != 0) {
        success = 0;
    }
    if (success && rename(temporaryFilename, filename) != 0) {
        success = 0;
    }
    if (!success) {
        remove(temporaryFilename);
    }

    free(temporaryFilename);
    return success;
}

void freeTree(Node* root)
{
    if (!root) {
        return;
    }
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

static void removeLineEnding(char* line)
{
    char* lineEnding = strpbrk(line, "\r\n");
    if (lineEnding) {
        *lineEnding = '\0';
    }
}

static void discardLineRemainder(FILE* file)
{
    int character;
    do {
        character = fgetc(file);
    } while (character != '\n' && character != EOF);
}

int loadAirports(const char* filename, Node** root)
{
    if (!filename || !root) {
        return -1;
    }

    FILE* file = fopen(filename, "r");
    if (!file) {
        return -1;
    }

    char line[512];
    int count = 0;
    int success = 1;

    while (fgets(line, (int)sizeof(line), file)) {
        if (!strchr(line, '\n') && !feof(file)) {
            discardLineRemainder(file);
            success = 0;
            break;
        }
        removeLineEnding(line);

        char* colon = strchr(line, ':');
        if (!colon || colon == line || colon[1] == '\0') {
            success = 0;
            break;
        }
        *colon = '\0';

        Airport airport;
        if (!normalizeIataCode(line, airport.code)
            || strlen(colon + 1) >= sizeof(airport.name)) {
            success = 0;
            break;
        }
        snprintf(airport.name, sizeof(airport.name), "%s", colon + 1);

        int inserted = insertAirport(root, &airport);
        if (inserted < 0) {
            success = 0;
            break;
        }
        count += inserted;
    }

    if (ferror(file)) {
        success = 0;
    }
    if (fclose(file) != 0) {
        success = 0;
    }
    return success ? count : -1;
}
