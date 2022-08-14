#include <bits/stdc++.h>
using namespace std;

#ifndef INPUTOUTPUT_H
#define INPUTOUTPUT_H

struct Segment
{
    bool is_line; // 0 = arc, 1 = line
    float x1;
    float y1;
    float x2;
    float y2;
    float x_min, x_max, y_min, y_max; // the bounding box of the segment

    // below are only used when is_line = 1
    float slope;       //斜率
    float y_intercept; //截距
    double theta;      // the angle reference to positive x axis

    // below are only used when is_line = 0
    float center_x;
    float center_y;
    bool direction; // 0 = ClockWise(CW), 1 = ConterClockwise(CCW)
    double theta_1; // 圓心到點一角度
    double theta_2; // 圓心到點二角度
};

struct Copper // 外擴Copper
{
    float x_min, x_max, y_min, y_max;
    vector<Segment> segment;
};

struct Point
{
    float x;
    float y;
    bool Next_Arc; // if the point connected to arc
};

class Input_Output
{
public:
    float File_to_Parameter(const string); // 讀入參數
    vector<Segment> Read_Assembly(fstream &);
    vector<vector<Segment>> Read_Copper(fstream &);
    Segment String_to_Line(string);
    const vector<string> split(const string &, const char &);
    vector<Segment> Assembly_Buffer(const vector<Segment>, float, float);
    bool point_in_polygon(Point, vector<Point>, vector<vector<Point>>);
    vector<vector<Point>> Arc_Optimization(vector<Segment>);
    vector<Point> Line_to_Point(const vector<Segment>); // 將線段切割成點
private:
    vector<Point> Point_Extension(const vector<Segment>, const bool, float, float);
    vector<Segment> Point_to_Line(vector<Point>, vector<Segment>); // assembly 專屬
    vector<Point> Arc_to_Poly(Segment);

    vector<Point> Arc_Point_Tuning(const vector<Segment>, const bool, vector<Point>, float, float);
    Point first_intersection_between_line_and_arc_for_arc_tuning(Segment, Point, Point);
    Point first_intersection_between_arc_and_arc_for_arc_tuning(Segment, Segment);

    float Dot(Point, Point);
};

#endif