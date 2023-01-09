#include <bits/stdc++.h>
#include "FileIO.h"
#include "Buffer.h"
#include "Graph.h"

using namespace std;

#ifndef SCORER_H
#define SCORER_H

/*struct Segment
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
};*/

class Scorer
{
    /*
    friend Point operator-(Point, Point);
    friend Point operator+(Point, Point);
    friend Point operator*(double, Point);
    friend Point operator/(Point, double);
    friend bool operator==(Point, Point);
    friend bool operator!=(Point, Point);
    */
public:
    Scorer();
    Scorer(const char *, const char *);

    bool coppergap_not_valid, assemblygap_not_valid;

    void read_file();
    double first_quarter();
    double second_quarter();
    double third_quarter();
    double fourth_quarter();
    double Total_score(bool);
    Scorer &setAssembly(const Graph);
    Scorer &setCopper(const vector<Graph>);
    Scorer &setSilkscreen(const Graph);

private:
    FileIO file;
    GRAPH graph_op;
    POINT point_op;
    double Arc_Degree(const Segment &S1);

    double assemblygap, coppergap, silkscreenlen;
    // vector<int> continue_num; // 每個連續線段的線段數量
    Graph assembly;
    vector<Graph> copper;
    Graph silkscreen;
    Graph Assembly_push_out;
    bool ShowDetail;

    // third quarter and fourth quarter functions
    int dir(const Point, const Point, const Point);
    double disMin(const Point, const Point, const Point);                   //點P到線段AB的最短距離
    double Point_to_Arc_MinDist(Point, Segment);                            //點到圓弧之最短距離
    vector<Point> intersection_between_CentersLine_and_Arc(Segment, Point); // 圓心線對Arc的交點
    Point find_arbitary_point_on_arc(Segment);                              //找出Arc兩端外圓上一點
    bool On_Arc(Segment, Point);                                            //判斷點P是否在Arc上
    bool Concentric_Circle_On_Arc(Segment, Segment);                        //同心圓對兩Arc端點射線，在Arc是否有交點
};

#endif