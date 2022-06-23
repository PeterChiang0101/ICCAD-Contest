// segment.h
// declearation of class segment
// functions for segment is decleared in segment.cpp
#include <bits/stdc++.h>
using namespace std;

#ifndef SEGMENT_H
#define SEGMENT_H

class Segment
{
public:
    Segment();
    Segment String_to_Line(string);
    bool is_line; // 0 = arc, 1 = line
    float x1;
    float y1;
    float x2;
    float y2;
    float slope;       //斜率
    float y_intercept; //截距
    double theta;      // the angle reference to positive x axis
    // below is needed by arc, when deals with line set all to 0
    float center_x;
    float center_y;
    bool direction; // 0 = ClockWise(CW), 1 = ConterClockwise(CCW)
    double theta_1; // 圓心到點一角度
    double theta_2; // 圓心到點二角度
private:
    vector<string> split(const string &, const char &);
};

#endif
