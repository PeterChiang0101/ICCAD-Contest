// Copper.h
// declearation of struct Copper

#include <bits/stdc++.h>
#include "Segment.h"

using namespace std;

#ifndef COPPER_H
#define COPPER_H

struct Copper // 外擴Copper
{
    float x_min, x_max, y_min, y_max;
    vector<Segment> segment;
};

#endif // COPPER_H