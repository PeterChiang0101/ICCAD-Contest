// Point.hpp
// declearation of struct Point

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

#endif // POINT_H