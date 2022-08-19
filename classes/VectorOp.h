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
    static float cross(const Point, const Point); // 向量外積
    static double cross1(const Point, const Point, const Point); //共起點向量外積
    static float cross_point (const Point, const Point, const Point);
    static float dot(const Point, const Point);   // 向量積
    static Point orth_Cswap(const Point); //垂直順時鐘(向量)
    static Point orth_CCswap(const Point); //垂直逆時鍾(向量)
};

#endif // VECTOROP_H