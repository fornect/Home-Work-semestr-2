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

static int testNormalization(void)
{
    char code[IATA_CODE_CAPACITY];
    CHECK(avlTreeNormalizeCode("svo", code));
    CHECK(strcmp(code, "SVO") == 0);
    CHECK(!avlTreeNormalizeCode("SV", code));
    CHECK(!avlTreeNormalizeCode("SV00", code));
    CHECK(!avlTreeNormalizeCode("S1O", code));
    CHECK(!avlTreeNormalizeCode("СВО", code));
    return 1;
}

static int testTreeOperations(AvlTree* tree)
{
    const char* codes[] = { "MIA", "JFK", "SVO", "AMS", "LED", "AAA", "ZZZ", "DME", "CDG" };
    const size_t CODE_COUNT = sizeof(codes) / sizeof(codes[0]);

    for (size_t i = 0; i < CODE_COUNT; ++i) {
        char name[AIRPORT_NAME_CAPACITY];
        snprintf(name, sizeof(name), "Airport %s", codes[i]);
        CHECK(avlTreeInsert(tree, codes[i], name) == AvlTreeChanged);
    }
    CHECK(avlTreeSize(tree) == CODE_COUNT);
    CHECK(avlTreeInsert(tree, "svo", "Duplicate") == AvlTreeNotChanged);
    CHECK(avlTreeSize(tree) == CODE_COUNT);
    CHECK(strcmp(avlTreeFind(tree, "led"), "Airport LED") == 0);
    CHECK(avlTreeFind(tree, "XXX") == NULL);
    CHECK(avlTreeInsert(tree, "A1C", "Invalid") == AvlTreeError);
    CHECK(avlTreeInsert(tree, "ABC", "") == AvlTreeError);

    const char* deletions[] = { "AAA", "JFK", "MIA", "ZZZ" };
    const size_t DELETION_COUNT = sizeof(deletions) / sizeof(deletions[0]);
    for (size_t i = 0; i < DELETION_COUNT; ++i) {
        CHECK(avlTreeDelete(tree, deletions[i]) == AvlTreeChanged);
        CHECK(avlTreeFind(tree, deletions[i]) == NULL);
    }
    CHECK(avlTreeDelete(tree, "XXX") == AvlTreeNotChanged);
    CHECK(avlTreeSize(tree) == CODE_COUNT - DELETION_COUNT);
    return 1;
}

static int writeTextFile(const char* filename, const char* contents)
{
    FILE* file = fopen(filename, "w");
    if (!file) {
        return 0;
    }

    int success = fputs(contents, file) >= 0;
    if (fclose(file) != 0) {
        success = 0;
    }
    return success;
}

static int textFileEquals(const char* filename, const char* expected)
{
    FILE* file = fopen(filename, "r");
    if (!file) {
        return 0;
    }

    char contents[32];
    int success = fgets(contents, (int)sizeof(contents), file) != NULL;
    if (fclose(file) != 0) {
        success = 0;
    }
    return success && strcmp(contents, expected) == 0;
}

static int testPersistence(const AvlTree* tree, const char* filename)
{
    char collisionFilename[1024];
    snprintf(collisionFilename, sizeof(collisionFilename), "%s.tmp.0", filename);
    CHECK(writeTextFile(collisionFilename, "do not overwrite\n"));
    CHECK(avlTreeSave(tree, filename));

    CHECK(textFileEquals(collisionFilename, "do not overwrite\n"));

    AvlTree* loaded = avlTreeCreate();
    CHECK(loaded != NULL);
    CHECK(avlTreeLoad(loaded, filename) == (int)avlTreeSize(tree));
    CHECK(strcmp(avlTreeFind(loaded, "SVO"), "Airport SVO") == 0);

    char badFilename[1024];
    snprintf(badFilename, sizeof(badFilename), "%s.bad", filename);
    CHECK(writeTextFile(badFilename, "AAA:Valid Airport\nA1C:Invalid Airport\n"));
    CHECK(avlTreeLoad(loaded, badFilename) == -1);
    CHECK(strcmp(avlTreeFind(loaded, "SVO"), "Airport SVO") == 0);
    avlTreeDestroy(loaded);

    CHECK(remove(filename) == 0);
    CHECK(remove(collisionFilename) == 0);
    CHECK(remove(badFilename) == 0);
    return 1;
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Не указан путь к временному файлу теста.\n");
        return EXIT_FAILURE;
    }

    AvlTree* tree = avlTreeCreate();
    if (!tree) {
        return EXIT_FAILURE;
    }

    int success = testNormalization()
        && testTreeOperations(tree)
        && testPersistence(tree, argv[1]);
    avlTreeDestroy(tree);

    if (!success) {
        remove(argv[1]);
        return EXIT_FAILURE;
    }
    puts("Все тесты АВЛ-дерева пройдены.");
    return EXIT_SUCCESS;
}
