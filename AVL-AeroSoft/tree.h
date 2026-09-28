#pragma once

#include <stddef.h>

#define IATA_CODE_LENGTH 3
#define IATA_CODE_CAPACITY (IATA_CODE_LENGTH + 1)
#define AIRPORT_NAME_CAPACITY 256

/** Opaque AVL tree containing airport records. */
typedef struct AvlTree AvlTree;

/** Result of an operation that may or may not change the tree. */
typedef enum AvlTreeResult {
    AvlTreeError = -1,
    AvlTreeNotChanged = 0,
    AvlTreeChanged = 1
} AvlTreeResult;

/**
 * Creates an empty airport tree.
 *
 * @return A new tree, or NULL when memory allocation fails.
 */
AvlTree* avlTreeCreate(void);

/**
 * Frees the tree and all airport records stored in it.
 *
 * Passing NULL is allowed.
 */
void avlTreeDestroy(AvlTree* tree);

/**
 * Validates and normalizes a three-letter IATA code.
 *
 * @param input Code in either upper or lower case.
 * @param output Buffer receiving the upper-case code.
 * @return 1 for a valid code, otherwise 0.
 */
int avlTreeNormalizeCode(const char* input, char output[IATA_CODE_CAPACITY]);

/**
 * Adds an airport to the tree.
 *
 * @return AvlTreeChanged when inserted, AvlTreeNotChanged for a duplicate,
 *         or AvlTreeError for invalid data or an allocation failure.
 */
AvlTreeResult avlTreeInsert(AvlTree* tree, const char* code, const char* name);

/**
 * Deletes an airport by IATA code.
 *
 * @return AvlTreeChanged when deleted, AvlTreeNotChanged when absent,
 *         or AvlTreeError for invalid arguments.
 */
AvlTreeResult avlTreeDelete(AvlTree* tree, const char* code);

/**
 * Finds an airport name by IATA code.
 *
 * The returned pointer belongs to the tree and remains valid until that record
 * is deleted or the tree is destroyed.
 *
 * @return Airport name, or NULL when the code is invalid or not found.
 */
const char* avlTreeFind(const AvlTree* tree, const char* code);

/** Returns the number of airports currently stored in the tree. */
size_t avlTreeSize(const AvlTree* tree);

/**
 * Replaces the tree contents with records loaded from a code:name text file.
 *
 * The original tree remains unchanged if loading fails.
 *
 * @return Number of unique loaded records, or -1 on an error.
 */
int avlTreeLoad(AvlTree* tree, const char* filename);

/**
 * Saves all records to a code:name text file in code order.
 *
 * A temporary file is used so a failed save does not damage the original.
 *
 * @return 1 on success, otherwise 0.
 */
int avlTreeSave(const AvlTree* tree, const char* filename);
