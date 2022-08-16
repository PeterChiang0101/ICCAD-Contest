// VectorOp.hpp
// declearation of struct VectorOp

#include <bits/stdc++.h>
#include "Segment.h"
#include "Point.h"

using namespace std;

#ifndef VECTOROP_H
#define VECTOROP_H

class VectorOp // Copper Assembly共用
{
public:
    float cross(Point, Point); // 向量外積
    float dot(Point, Point);   // 向量積
    Point operator-(Point , Point );
    Point operator+(Point a, Point b);
    Point operator*(double c, Point a);
    Point operator/ (Point a, double c);
};

#endif // VECTOROP_H