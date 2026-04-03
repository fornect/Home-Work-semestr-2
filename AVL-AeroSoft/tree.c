#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct BSTNode {
    char iataCode[3];
    char name[100];
    struct BSTNode* parent;
    struct BSTNode* left;
    struct BSTNode* right;
} BSTNode;

typedef struct BST {
    BSTNode* root;
    size_t cardinality;

} BST;

size_t bstSize(const BST* tree)
{
    return tree->cardinality;
}

int bstInsertTree(BST* tree, char* iataCode, char* name)
{
    BSTNode** insert = &tree->root;
    BSTNode* parent = NULL;

    while (*insert != NULL) {
        parent = *insert;

        if (strcmp(iataCode, (**insert).iataCode) < 0) {
            insert = &(**insert).left;
        } else if (strcmp(iataCode, (**insert).iataCode) > 0) {
            insert = &(**insert).right;
        } else {
            return 0;
        }
    }

    BSTNode* newNode = malloc(sizeof(BSTNode));

    if (newNode == NULL) {
        return ENOMEM;
    }

    *newNode = (BSTNode) {
        .iataCode = *iataCode,
        .name = *name,
        .parent = parent,
        .left = NULL,
        .right = NULL
    };

    *insert = newNode;
    tree->cardinality++;

    return 1;
}

void bstNodeFree(BSTNode** node)
{
    if (*node == NULL) {
        return;
    }

    bstNodeFree(&(**node).left);
    bstNodeFree(&(**node).right);
    free(*node);
    *node = NULL;
}

void bstFree(BST* tree)
{
    tree->cardinality = 0;
    bstNodeFree(&tree->root);
    tree->root = NULL;
}

void bstDelete(BST* tree, char* iataCode)
{

    // node search
    BSTNode* current = tree->root;

    while (current != NULL) {
        if (current->iataCode == iataCode) {
            break;
        }

        if (current->iataCode > iataCode) {
            current = current->left;
        } else {
            current = current->right;
        }
    }

    // There is no such iataCode.
    if (current == NULL) {
        return;
    }

    // node == leaf
    if (current->left == NULL && current->right == NULL) {
        BSTNode* parent = current->parent;

        if (parent == NULL) {
            tree->root = NULL;
        } else {
            if (parent->left == current) {
                parent->left = NULL;
            } else {
                parent->right = NULL;
            }
        }

        free(current);
        tree->cardinality--;

        return;
    }
}