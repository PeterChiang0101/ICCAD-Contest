// VectorOp.h
// implementation of VectorOp functions

#include <bits/stdc++.h>
#include "VectorOp.h"
#include "Point.h"


using namespace std;

double VectorOp::cross(const Point v1, const Point v2) // 向量外積
{
    // 沒有除法，儘量避免誤差。
    return v1.x * v2.y - v1.y * v2.x;
}

double VectorOp::cross1(const Point o, const Point a, const Point b) //共起點向量外積
{
    return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
}

double VectorOp::cross_point (const Point original, const Point a, const Point b)
{
    return (a.x - original.x) * (b.y - original.y) - (a.y - original.y) * (b.x - original.x);
}

double VectorOp::dot(const Point v1, const Point v2) // 向量內積
{
    return v1.x * v2.x + v1.y * v2.y;
}

//向量伸縮平移

Point VectorOp::orth_Cswap(const Point a) //垂直順時鐘(向量)
{
    Point v;
    v.x = a.y * (-1);
    v.y = a.x;
    return v;
}

Point VectorOp::orth_CCswap(const Point a) //垂直逆時鍾(向量)
{
    Point v;
    v.x = a.y;
    v.y = a.x * (-1);
    return v;
}
