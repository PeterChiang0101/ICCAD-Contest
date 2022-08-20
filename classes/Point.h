// Point.hpp
// declearation of struct Point
#include <bits/stdc++.h>

using namespace std;

#ifndef POINT_H
#define POINT_H

struct Point
{
    float x;
    float y;
    bool Next_Arc; // if the point connected to arc
};

struct Point_ID
{
    int ID{-1};//record the detaol of point intersection segment
    Point point;
};

class POINT 
{
    public:
        static double dis2(const Point, const Point);//兩點距離平方
        static double dist(const Point, const Point);//兩點距離
        bool point_in_polygon(const Point, const vector<Point>, const vector<vector<Point>>) ;// 運用射線法判斷點在圖形內外
        bool Point_Inside_Arc(const float, float, float, const bool); // 確認點是否在圓弧內，已確認在該圓弧所屬的園內 (counterclockwise)
        Point intersection(const Point,const Point,const Point, const Point);//利用兩直線端點找交點
        bool In_Between_Lines(const Point, const Point, const Point);
    private:
        double interpolate_x(const float, const Point, const Point); // 待測點與圖形邊界交會的x值
};

#endif // POINT_H