#pragma once

typedef struct Road {
    int firstCity;
    int secondCity;
    long long length;
} Road;

int distributeCities(int cityCount, const Road* roads, int roadCount,
    const int* capitals, int stateCount, int* owners);
