#include "empire.h"

#include <limits.h>

static int findNearestCity(const Road* roads, int roadCount, const int* owners, int state)
{
    int nearestCity = 0;
    long long nearestDistance = LLONG_MAX;

    for (int i = 0; i < roadCount; ++i) {
        int city = 0;
        if (owners[roads[i].firstCity] == state && owners[roads[i].secondCity] == 0) {
            city = roads[i].secondCity;
        } else if (owners[roads[i].secondCity] == state && owners[roads[i].firstCity] == 0) {
            city = roads[i].firstCity;
        }

        if (city != 0
            && (roads[i].length < nearestDistance
                || (roads[i].length == nearestDistance && city < nearestCity))) {
            nearestCity = city;
            nearestDistance = roads[i].length;
        }
    }
    return nearestCity;
}

int distributeCities(int cityCount, const Road* roads, int roadCount,
    const int* capitals, int stateCount, int* owners)
{
    if (cityCount <= 0 || roadCount < 0 || stateCount <= 0 || stateCount > cityCount
        || !capitals || !owners || (roadCount > 0 && !roads)) {
        return 0;
    }

    for (int city = 0; city <= cityCount; ++city) {
        owners[city] = 0;
    }

    for (int state = 1; state <= stateCount; ++state) {
        int capital = capitals[state - 1];
        if (capital < 1 || capital > cityCount || owners[capital] != 0) {
            return 0;
        }
        owners[capital] = state;
    }

    int distributedCount = stateCount;
    while (distributedCount < cityCount) {
        int citiesAdded = 0;
        for (int state = 1; state <= stateCount && distributedCount < cityCount; ++state) {
            int nearestCity = findNearestCity(roads, roadCount, owners, state);
            if (nearestCity != 0) {
                owners[nearestCity] = state;
                ++distributedCount;
                ++citiesAdded;
            }
        }

        if (citiesAdded == 0) {
            return 0;
        }
    }
    return 1;
}
