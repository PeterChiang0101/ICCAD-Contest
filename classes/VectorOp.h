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
};

#endif // VECTOROP_H