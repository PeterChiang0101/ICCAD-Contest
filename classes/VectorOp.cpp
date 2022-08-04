// VectorOp.h
// implementation of VectorOp functions

#include <bits/stdc++.h>
#include "VectorOp.h"
#include "Point.h"


using namespace std;

float VectorOp::cross(Point v1, Point v2) // 向量外積
{
    // 沒有除法，儘量避免誤差。
    return v1.x * v2.y - v1.y * v2.x;
}

float VectorOp::dot(Point v1, Point v2) // 向量積
{
    return v1.x * v2.x + v1.y * v2.y;
}