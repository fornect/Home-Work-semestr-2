#include "empire.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

enum {
    MaxCityCount = 1000000,
    MaxRoadCount = 5000000,
};

static size_t calculateCapacity(int elementCount)
{
    size_t capacity = 1;
    while (capacity < (size_t)elementCount) {
        capacity *= 2;
    }
    return capacity;
}

static int readGraph(FILE* input, int* cityCount, Road** roads, int* roadCount)
{
    if (fscanf(input, "%d%d", cityCount, roadCount) != 2
        || *cityCount <= 0 || *cityCount > MaxCityCount
        || *roadCount < 0 || *roadCount > MaxRoadCount
        || (size_t)*roadCount > SIZE_MAX / sizeof(**roads)) {
        return 0;
    }

    *roads = NULL;
    if (*roadCount > 0) {
        size_t capacity = calculateCapacity(*roadCount);
        *roads = malloc(capacity * sizeof(**roads));
        if (!*roads) {
            return 0;
        }
    }

    for (int i = 0; i < *roadCount; ++i) {
        Road* road = &(*roads)[i];
        if (fscanf(input, "%d%d%lld", &road->firstCity, &road->secondCity, &road->length) != 3
            || road->firstCity < 1 || road->firstCity > *cityCount
            || road->secondCity < 1 || road->secondCity > *cityCount
            || road->firstCity == road->secondCity || road->length < 0) {
            free(*roads);
            *roads = NULL;
            return 0;
        }
    }
    return 1;
}

static int readCapitals(FILE* input, int cityCount, int* stateCount, int** capitals)
{
    if (fscanf(input, "%d", stateCount) != 1
        || *stateCount <= 0 || *stateCount > cityCount
        || (size_t)*stateCount > SIZE_MAX / sizeof(**capitals)) {
        return 0;
    }

    size_t capacity = calculateCapacity(*stateCount);
    *capitals = malloc(capacity * sizeof(**capitals));
    if (!*capitals) {
        return 0;
    }

    for (int i = 0; i < *stateCount; ++i) {
        if (fscanf(input, "%d", &(*capitals)[i]) != 1) {
            free(*capitals);
            *capitals = NULL;
            return 0;
        }
    }
    return 1;
}

static void printStates(const int* owners, int cityCount, int stateCount)
{
    for (int state = 1; state <= stateCount; ++state) {
        printf("Государство %d:", state);
        for (int city = 1; city <= cityCount; ++city) {
            if (owners[city] == state) {
                printf(" %d", city);
            }
        }
        putchar('\n');
    }
}

int main(int argc, char* argv[])
{
    if (argc > 2) {
        fprintf(stderr, "Использование: %s [входной_файл]\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE* input = stdin;
    if (argc == 2) {
        input = fopen(argv[1], "r");
        if (!input) {
            fprintf(stderr, "Ошибка: не удалось открыть файл %s.\n", argv[1]);
            return EXIT_FAILURE;
        }
    }

    int cityCount;
    int roadCount;
    Road* roads = NULL;
    int stateCount;
    int* capitals = NULL;

    int success = readGraph(input, &cityCount, &roads, &roadCount)
        && readCapitals(input, cityCount, &stateCount, &capitals);
    if (argc == 2) {
        fclose(input);
    }
    if (!success) {
        fprintf(stderr, "Ошибка: некорректные входные данные.\n");
        free(roads);
        free(capitals);
        return EXIT_FAILURE;
    }

    size_t ownerCapacity = calculateCapacity(cityCount + 1);
    int* owners = calloc(ownerCapacity, sizeof(*owners));
    if (!owners || !distributeCities(cityCount, roads, roadCount, capitals, stateCount, owners)) {
        fprintf(stderr, "Ошибка: граф несвязный или столицы заданы некорректно.\n");
        free(roads);
        free(capitals);
        free(owners);
        return EXIT_FAILURE;
    }

    printStates(owners, cityCount, stateCount);
    free(roads);
    free(capitals);
    free(owners);
    return EXIT_SUCCESS;
}
