// Graph.h
// declearation of struct Graph

#include <bits/stdc++.h>
#include "Segment.h"

using namespace std;

#ifndef GRAPH_H
#define GRAPH_H

struct Graph // Copper Assembly共用
{
    float x_min, x_max, y_min, y_max;
    vector<Segment> segment;
};

#endif // GRAPH_H