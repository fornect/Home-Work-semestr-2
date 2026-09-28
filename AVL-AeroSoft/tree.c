#include "tree.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

struct AvlTree {
    Node* root;
    size_t size;
};

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

static Node* insertRecursive(Node* root, const Airport* airport, AvlTreeResult* result)
{
    if (!root) {
        Node* node = createNode(airport);
        *result = node ? AvlTreeChanged : AvlTreeError;
        return node;
    }

    int comparison = strcmp(airport->code, root->airport.code);
    if (comparison == 0) {
        *result = AvlTreeNotChanged;
        return root;
    }

    if (comparison < 0) {
        Node* left = insertRecursive(root->left, airport, result);
        if (*result == AvlTreeError) {
            return root;
        }
        root->left = left;
    } else {
        Node* right = insertRecursive(root->right, airport, result);
        if (*result == AvlTreeError) {
            return root;
        }
        root->right = right;
    }

    return rebalance(root);
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
        root->right = deleteRecursive(root->right, root->airport.code, deleted);
    }

    return rebalance(root);
}

static const Node* findNode(const Node* root, const char* code)
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

static void destroyNodes(Node* root)
{
    if (!root) {
        return;
    }

    destroyNodes(root->left);
    destroyNodes(root->right);
    free(root);
}

AvlTree* avlTreeCreate(void)
{
    return calloc(1, sizeof(AvlTree));
}

void avlTreeDestroy(AvlTree* tree)
{
    if (!tree) {
        return;
    }

    destroyNodes(tree->root);
    free(tree);
}

int avlTreeNormalizeCode(const char* input, char output[IATA_CODE_CAPACITY])
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

AvlTreeResult avlTreeInsert(AvlTree* tree, const char* code, const char* name)
{
    if (!tree || !name || *name == '\0' || strlen(name) >= AIRPORT_NAME_CAPACITY) {
        return AvlTreeError;
    }

    Airport airport;
    if (!avlTreeNormalizeCode(code, airport.code)) {
        return AvlTreeError;
    }
    snprintf(airport.name, sizeof(airport.name), "%s", name);

    AvlTreeResult result = AvlTreeNotChanged;
    tree->root = insertRecursive(tree->root, &airport, &result);
    if (result == AvlTreeChanged) {
        ++tree->size;
    }
    return result;
}

AvlTreeResult avlTreeDelete(AvlTree* tree, const char* code)
{
    char normalizedCode[IATA_CODE_CAPACITY];
    if (!tree || !avlTreeNormalizeCode(code, normalizedCode)) {
        return AvlTreeError;
    }

    int deleted = 0;
    tree->root = deleteRecursive(tree->root, normalizedCode, &deleted);
    if (!deleted) {
        return AvlTreeNotChanged;
    }

    --tree->size;
    return AvlTreeChanged;
}

const char* avlTreeFind(const AvlTree* tree, const char* code)
{
    char normalizedCode[IATA_CODE_CAPACITY];
    if (!tree || !avlTreeNormalizeCode(code, normalizedCode)) {
        return NULL;
    }

    const Node* found = findNode(tree->root, normalizedCode);
    return found ? found->airport.name : NULL;
}

size_t avlTreeSize(const AvlTree* tree)
{
    return tree ? tree->size : 0;
}

static int writeNodes(const Node* root, FILE* file)
{
    if (!root) {
        return 1;
    }

    return writeNodes(root->left, file)
        && fprintf(file, "%s:%s\n", root->airport.code, root->airport.name) >= 0
        && writeNodes(root->right, file);
}

static FILE* createTemporaryFile(const char* filename, char** temporaryFilename)
{
    const unsigned int ATTEMPT_LIMIT = 100;
    size_t capacity = strlen(filename) + sizeof(".tmp.99");
    char* candidate = malloc(capacity);
    if (!candidate) {
        return NULL;
    }

    FILE* file = NULL;
    for (unsigned int attempt = 0; attempt < ATTEMPT_LIMIT; ++attempt) {
        snprintf(candidate, capacity, "%s.tmp.%u", filename, attempt);
        errno = 0;
        file = fopen(candidate, "wx");
        if (file || errno != EEXIST) {
            break;
        }
    }

    if (!file) {
        free(candidate);
        return NULL;
    }
    *temporaryFilename = candidate;
    return file;
}

int avlTreeSave(const AvlTree* tree, const char* filename)
{
    if (!tree || !filename) {
        return 0;
    }

    char* temporaryFilename = NULL;
    FILE* file = createTemporaryFile(filename, &temporaryFilename);
    if (!file) {
        return 0;
    }

    int success = writeNodes(tree->root, file);
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

static int readAirports(FILE* file, AvlTree* destination)
{
    char line[512];
    while (fgets(line, (int)sizeof(line), file)) {
        if (!strchr(line, '\n') && !feof(file)) {
            discardLineRemainder(file);
            return 0;
        }
        removeLineEnding(line);

        char* colon = strchr(line, ':');
        if (!colon || colon == line || colon[1] == '\0') {
            return 0;
        }
        *colon = '\0';

        AvlTreeResult result = avlTreeInsert(destination, line, colon + 1);
        if (result == AvlTreeError) {
            return 0;
        }
    }
    return !ferror(file);
}

int avlTreeLoad(AvlTree* tree, const char* filename)
{
    if (!tree || !filename) {
        return -1;
    }

    FILE* file = fopen(filename, "r");
    if (!file) {
        return -1;
    }

    AvlTree loaded = { 0 };
    int success = readAirports(file, &loaded);
    if (fclose(file) != 0) {
        success = 0;
    }
    if (!success || loaded.size > (size_t)INT_MAX) {
        destroyNodes(loaded.root);
        return -1;
    }

    destroyNodes(tree->root);
    tree->root = loaded.root;
    tree->size = loaded.size;
    return (int)tree->size;
}
