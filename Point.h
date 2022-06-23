#include <bits/stdc++.h>
using namespace std;

#ifndef POINT_H
#define POINT_H

class Point
{
public:
    vector<Point> Point_Extension(const vector<Segment>, const bool);

private:
    float x;
    float y;
    bool Next_Arc; // if the point connected to arc
};

#endif