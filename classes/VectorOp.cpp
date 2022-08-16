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

float VectorOp::dot(Point v1, Point v2) // 向量內積
{
    return v1.x * v2.x + v1.y * v2.y;
}

float VectorOp::cross_point (Point original, Point a, Point b)
{
    return (a.x - original.x) * (b.y - original.y) - (a.y - original.y) * (b.x - original.x);
}

//向量伸縮平移
Point operator-(Point a, Point b)
{
    Point v;
    v.x = a.x - b.x;
    v.y = a.y - b.y;
    return v;
}

Point operator+(Point a, Point b)
{
    Point v;
    v.x = a.x + b.x;
    v.y = a.y + b.y;
    return v;
}

Point operator*(double c, Point a) //@overload,伸縮
{
    Point v;
    v.x = c * a.x;
    v.y = c * a.y;
    return v;
}

Point operator/(Point a, double c) //@overload,伸縮
{
    Point v;
    v.x = a.x / c;
    v.y = a.y / c;
    return v;
}