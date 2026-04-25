#include <ctype.h>
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

int height(Node* n)
{
    if (n) {
        return n->height;
    } else {
        return 0;
    }
}

int getBalance(Node* n)
{
    if (n) {
        return height(n->left) - height(n->right);
    } else {
        return 0;
    }
}

Node* createNode(Airport airport)
{
    Node* node = (Node*)malloc(sizeof(Node));
    node->airport = airport;
    node->height = 1;
    node->left = node->right = NULL;
    return node;
}

Node* rotateRight(Node* y)
{
    if (!y || !y->left)
        return y;

    Node* x = y->left;
    Node* z = x->right;

    x->right = y;
    y->left = z;

    int leftHeightY, rightHeightY;
    leftHeightY = height(y->left);
    rightHeightY = height(y->right);
    if (leftHeightY > rightHeightY) {
        y->height = leftHeightY + 1;
    } else {
        y->height = rightHeightY + 1;
    }

    int leftHeightX, rightHeightX;
    leftHeightX = height(x->left);
    rightHeightX = height(x->right);
    if (leftHeightX > rightHeightX) {
        x->height = leftHeightX + 1;
    } else {
        x->height = rightHeightX + 1;
    }

    return x;
}

Node* rotateLeft(Node* x)
{
    if (!x || !x->right)
        return x;

    Node* y = x->right;
    Node* z = y->left;

    y->left = x;
    x->right = z;

    int leftHeightX, rightHeightX;
    leftHeightX = height(x->left);
    rightHeightX = height(x->right);
    if (leftHeightX > rightHeightX) {
        x->height = leftHeightX + 1;
    } else {
        x->height = rightHeightX + 1;
    }

    int leftHeightY, rightHeightY;
    leftHeightY = height(y->left);
    rightHeightY = height(y->right);
    if (leftHeightY > rightHeightY) {
        y->height = leftHeightY + 1;
    } else {
        y->height = rightHeightY + 1;
    }

    return y;
}

Node* insert(Node* node, Airport airport)
{
    if (!node) {
        return createNode(airport);
    }

    int cmp = strcmp(airport.code, node->airport.code);

    if (cmp < 0) {
        node->left = insert(node->left, airport);
    } else if (cmp > 0) {
        node->right = insert(node->right, airport);
    } else {
        return node;
    }

    int leftHeight, rightHeight;
    leftHeight = height(node->left);
    rightHeight = height(node->right);
    if (leftHeight > rightHeight) {
        node->height = leftHeight + 1;
    } else {
        node->height = rightHeight + 1;
    }

    int balance = getBalance(node);

    if (balance > 1 && node->left) {
        if (strcmp(airport.code, node->left->airport.code) < 0) {
            return rotateRight(node);
        } else {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
    }

    if (balance < -1 && node->right) {
        if (strcmp(airport.code, node->right->airport.code) > 0) {
            return rotateLeft(node);
        } else {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
    }
    return node;
}

Node* minValueNode(Node* node)
{
    Node* current = node;
    while (current->left) {
        current = current->left;
    }
    return current;
}

Node* deleteNode(Node* root, char* code)
{
    if (!root) {
        return root;
    }

    int cmp = strcmp(code, root->airport.code);

    if (cmp < 0) {
        root->left = deleteNode(root->left, code);
    } else if (cmp > 0) {
        root->right = deleteNode(root->right, code);
    } else {
        if (!root->left || !root->right) {
            Node* temp;
            if (root->left) {
                temp = root->left;
            } else {
                temp = root->right;
            }
            if (!temp) {
                temp = root;
                root = NULL;
            } else {
                *root = *temp;
            }
            free(temp);
        } else {
            Node* temp = minValueNode(root->right);
            root->airport = temp->airport;
            root->right = deleteNode(root->right, temp->airport.code);
        }
    }

    if (!root) {
        return root;
    }

    int leftHeight, rightHeight;
    leftHeight = height(root->left);
    rightHeight = height(root->right);
    if (leftHeight > rightHeight) {
        root->height = leftHeight + 1;
    } else {
        root->height = rightHeight + 1;
    }

    int balance = getBalance(root);

    if (balance > 1) {
        if (getBalance(root->left) < 0)
            root->left = rotateLeft(root->left);
        return rotateRight(root);
    }

    if (balance < -1) {
        if (getBalance(root->right) > 0)
            root->right = rotateRight(root->right);
        return rotateLeft(root);
    }
    return root;
}

Node* search(Node* root, char* code)
{
    if (!root || strcmp(root->airport.code, code) == 0) {
        return root;
    }
    if (strcmp(code, root->airport.code) < 0) {
        return search(root->left, code);
    }
    return search(root->right, code);
}

int countNodes(Node* root)
{
    if (!root) {
        return 0;
    }
    return 1 + countNodes(root->left) + countNodes(root->right);
}

void saveToFile(Node* root, FILE* file)
{
    if (root) {
        saveToFile(root->left, file);
        fprintf(file, "%s:%s\n", root->airport.code, root->airport.name);
        saveToFile(root->right, file);
    }
}

int saveTreeToFile(Node* root, const char* filename)
{
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Ошибка: Не удалось открыть файл %s для записи!\n", filename);
        return 0;
    }

    saveToFile(root, file);
    fclose(file);
    return 1;
}

void freeTree(Node* root)
{
    if (root) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}

void trimNewline(char* str)
{
    char* newline = strchr(str, '\n');
    if (newline) {
        *newline = '\0';
    }
}

void toUpperCase(char* str)
{
    if (str == NULL) {
        return;
    }
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = (char)toupper((unsigned char)str[i]);
    }
}

int loadAirports(const char* filename, Node** root)
{
    if (root == NULL) {
        return -1;
    }

    FILE* file = fopen(filename, "r");
    if (!file) {
        return -1;
    }

    char line[512];
    int count = 0;

    while (fgets(line, sizeof(line), file)) {
        trimNewline(line);

        char* colon = strchr(line, ':');
        if (colon) {
            Airport airport;
            *colon = '\0';
            strncpy(airport.code, line, sizeof(airport.code) - 1);
            airport.code[sizeof(airport.code) - 1] = '\0';
            strncpy(airport.name, colon + 1, sizeof(airport.name) - 1);
            airport.name[sizeof(airport.name) - 1] = '\0';
            *root = insert(*root, airport);
            count++;
        }
    }

    fclose(file);
    return count;
}