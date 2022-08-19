// Graph.h
// declearation of struct Graph

#include <bits/stdc++.h>
#include "Segment.h"

using namespace std;

#ifndef GRAPH_H
#define GRAPH_H

struct Graph // Copper Assembly共用
{
    float x_min, x_max, y_min, y_max;
    vector<Segment> segment;
};

class GRAPH 
{
    public:
        vector<Point> Line_to_Point(const Graph);            // 將線段切割成點
        vector<vector<Point>> Arc_Optimization(const Graph); // 將圓弧切割成多個點，以利辨識點在圖形內外
        Segment Arc_Boundary_Meas(const Segment);            // 判斷圓弧線段x,y極值

        Point intersection(const Point,const Point,const Point, const Point);//利用兩直線端點找交點
        Point first_intersection_between_line_and_arc_for_arc_tuning(const Segment, const Point, const Point); //找圓弧和直線第一個交點
        Point first_intersection_between_arc_and_arc_for_arc_tuning(const Segment, const Segment); //找圓弧和圓弧第一個交點
        vector<Point> intersection_between_line_and_arc(const Segment, const Point, const Point); //找圓弧和直線交點
        vector<Point> intersection_between_arc_and_arc(const Segment, const Segment); //找圓弧和圓弧交點
        bool Line_intersect(const Segment, const Segment);//找兩Segment交點

    private:
        vector<Point> Arc_to_Poly(Segment Arc);              // 被整合進"Arc_Optimization"
        VectorOp V_Op;
        POINT point;
};

#endif // GRAPH_H