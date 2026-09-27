#include "../src/empire.h"
#include <stdio.h>

static int ownersAreEqual(const int* actual, const int* expected, int cityCount)
{
    for (int city = 1; city <= cityCount; ++city) {
        if (actual[city] != expected[city]) {
            return 0;
        }
    }
    return 1;
}

static int testRoundRobinDistribution(void)
{
    const Road ROADS[] = {
        { 1, 3, 2 },
        { 1, 4, 7 },
        { 2, 4, 1 },
        { 2, 5, 4 },
        { 3, 6, 3 },
        { 4, 6, 2 },
        { 5, 6, 1 },
    };
    const int CAPITALS[] = { 1, 2 };
    int owners[7];
    const int EXPECTED[] = { 0, 1, 2, 1, 2, 2, 1 };

    if (!distributeCities(6, ROADS, 7, CAPITALS, 2, owners)) {
        return 0;
    }
    if (!ownersAreEqual(owners, EXPECTED, 6)) {
        return 0;
    }
    return 1;
}

static int testSkippedTurn(void)
{
    const Road ROADS[] = {
        { 1, 2, 1 },
        { 2, 3, 1 },
        { 3, 4, 1 },
    };
    const int CAPITALS[] = { 1, 2 };
    int owners[5];
    const int EXPECTED[] = { 0, 1, 2, 2, 2 };

    if (!distributeCities(4, ROADS, 3, CAPITALS, 2, owners)) {
        return 0;
    }
    if (!ownersAreEqual(owners, EXPECTED, 4)) {
        return 0;
    }
    return 1;
}

static int testEqualRoadLengths(void)
{
    const Road ROADS[] = {
        { 1, 4, 1 },
        { 1, 3, 1 },
        { 2, 3, 2 },
        { 2, 4, 2 },
    };
    const int CAPITALS[] = { 1, 2 };
    int owners[5];
    const int EXPECTED[] = { 0, 1, 2, 1, 2 };

    if (!distributeCities(4, ROADS, 4, CAPITALS, 2, owners)) {
        return 0;
    }
    if (!ownersAreEqual(owners, EXPECTED, 4)) {
        return 0;
    }
    return 1;
}

int main(void)
{
    int success = testRoundRobinDistribution()
        && testSkippedTurn()
        && testEqualRoadLengths();
    if (!success) {
        return 1;
    }

    puts("Все тесты распределения городов пройдены.");
    return 0;
}
