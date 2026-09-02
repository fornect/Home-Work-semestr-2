#include "tree.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition)                                                                    \
    do {                                                                                    \
        if (!(condition)) {                                                                 \
            fprintf(stderr, "Проверка не пройдена: %s, строка %d\n", #condition, __LINE__); \
            return 0;                                                                       \
        }                                                                                   \
    } while (0)

static int maxInt(int first, int second)
{
    return first > second ? first : second;
}

static int verifySubtree(const Node* root, const char* minimum, const char* maximum, int* height)
{
    if (!root) {
        *height = 0;
        return 1;
    }
    if ((minimum && strcmp(root->airport.code, minimum) <= 0)
        || (maximum && strcmp(root->airport.code, maximum) >= 0)) {
        return 0;
    }

    int leftHeight;
    int rightHeight;
    if (!verifySubtree(root->left, minimum, root->airport.code, &leftHeight)
        || !verifySubtree(root->right, root->airport.code, maximum, &rightHeight)) {
        return 0;
    }

    int difference = leftHeight - rightHeight;
    *height = 1 + maxInt(leftHeight, rightHeight);
    return difference >= -1 && difference <= 1 && root->height == *height;
}

static int verifyTree(const Node* root)
{
    int height;
    return verifySubtree(root, NULL, NULL, &height);
}

static Airport makeAirport(const char* code)
{
    Airport airport = { 0 };
    snprintf(airport.code, sizeof(airport.code), "%s", code);
    snprintf(airport.name, sizeof(airport.name), "Airport %s", code);
    return airport;
}

static int testNormalization(void)
{
    char code[IATA_CODE_CAPACITY];
    CHECK(normalizeIataCode("svo", code));
    CHECK(strcmp(code, "SVO") == 0);
    CHECK(!normalizeIataCode("SV", code));
    CHECK(!normalizeIataCode("SV00", code));
    CHECK(!normalizeIataCode("S1O", code));
    CHECK(!normalizeIataCode("СВО", code));
    return 1;
}

static int testTreeOperations(Node** root)
{
    const char* codes[] = { "MIA", "JFK", "SVO", "AMS", "LED", "AAA", "ZZZ", "DME", "CDG" };
    const size_t CODE_COUNT = sizeof(codes) / sizeof(codes[0]);

    for (size_t i = 0; i < CODE_COUNT; ++i) {
        Airport airport = makeAirport(codes[i]);
        CHECK(insertAirport(root, &airport) == 1);
        CHECK(verifyTree(*root));
    }
    CHECK(countNodes(*root) == CODE_COUNT);

    Airport duplicate = makeAirport("SVO");
    CHECK(insertAirport(root, &duplicate) == 0);
    CHECK(countNodes(*root) == CODE_COUNT);
    CHECK(searchAirport(*root, "LED") != NULL);
    CHECK(searchAirport(*root, "XXX") == NULL);

    const char* deletions[] = { "AAA", "JFK", "MIA", "ZZZ" };
    const size_t DELETION_COUNT = sizeof(deletions) / sizeof(deletions[0]);
    for (size_t i = 0; i < DELETION_COUNT; ++i) {
        CHECK(deleteAirport(root, deletions[i]) == 1);
        CHECK(searchAirport(*root, deletions[i]) == NULL);
        CHECK(verifyTree(*root));
    }
    CHECK(deleteAirport(root, "XXX") == 0);
    CHECK(countNodes(*root) == CODE_COUNT - DELETION_COUNT);
    return 1;
}

static int testPersistence(const Node* root, const char* filename)
{
    CHECK(saveTreeToFile(root, filename));

    Node* loaded = NULL;
    int loadedCount = loadAirports(filename, &loaded);
    CHECK(loadedCount == (int)countNodes(root));
    CHECK(verifyTree(loaded));
    CHECK(searchAirport(loaded, "SVO") != NULL);
    freeTree(loaded);

    FILE* duplicateFile = fopen(filename, "w");
    CHECK(duplicateFile != NULL);
    CHECK(fputs("AAA:First Airport\nAAA:Duplicate Airport\n", duplicateFile) >= 0);
    CHECK(fclose(duplicateFile) == 0);

    loaded = NULL;
    CHECK(loadAirports(filename, &loaded) == 1);
    CHECK(countNodes(loaded) == 1);
    freeTree(loaded);
    CHECK(remove(filename) == 0);
    return 1;
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Не указан путь к временному файлу теста.\n");
        return EXIT_FAILURE;
    }

    Node* root = NULL;
    int success = testNormalization()
        && testTreeOperations(&root)
        && testPersistence(root, argv[1]);
    freeTree(root);

    if (!success) {
        remove(argv[1]);
        return EXIT_FAILURE;
    }
    puts("Все тесты АВЛ-дерева пройдены.");
    return EXIT_SUCCESS;
}
