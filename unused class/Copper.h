#include <bits/stdc++.h>
#include "Segment.h"

using namespace std;

#ifndef COPPER_H
#define COPPER_H

class Copper
{
public:
    Copper();
    vector<vector<Segment>> Read_Copper(fstream &);
    vector<Copper> Copper_Buffer(const vector<vector<Segment>>);
    float x_min, x_max, y_min, y_max;

private:
    vector<vector<Segment>> copper_pack;
    vector<Segment> segment;
};

#endif