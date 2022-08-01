// Silkscreen.h
// declearation of class Silkscreen

#include "Graph.h"
#include "Segment.h"
#include "VectorOp.h"

#ifndef SILKSCREEN_H
#define SILKSCREEN_H

class Silkscreen
{
public:
    vector<Graph> Silkscreen_Assembly(const Graph, const Graph, const vector<Graph>);
    //                                      assembly    silkscreen      copper

private:
    VectorOp V_Op;
    vector<Segment> silkscreen_cut_single_copper(Segment, Graph);//not added to silkscreen.cpp (resvered)
    vector<Segment> Cut_Silkscreen_by_Copper(const Segment, const vector<Graph>);//not added to silkscreen.cpp (reserved)
    // in main.cpp : Final_Silkscreen()
    Graph Untuned_Silkscreen(const Graph, const vector<Graph>);
    vector<Graph> Delete_Short_Silkscreen(Graph);
    vector<Graph> fit_boarder_condition(vector<Graph>, Graph, Graph, vector<Graph>); //me
    vector<Graph> Add_Excess_Silkscreen_For_Boarder_Condition(vector<Graph>, Point, vector<Graph>, int, Graph);
    vector<vector<Point>> Arc_Optimization(Graph);
    vector<Point> Arc_to_Poly(Segment);
    vector<Point> Line_to_Point(const Graph);
    bool point_in_polygon(Point, vector<Point>, vector<vector<Point>>);
    float interpolate_x(float, Point, Point);
    bool Point_Inside_Arc(float, float, float, bool);
    Point intersection(Point, Point, Point, Point);
    bool In_Between_Lines(Point, Point, Point);
    vector<Point> intersection_between_line_and_arc(Segment, Point, Point);
    float Calculate_Silkscreen_length(const Graph&);//calculate the length of the Silkscreen
};

#endif // SILKSCREEN_H

//Note and issues:
/*
2022/7/29
`Find_Continuous_Segment(silkscreen)` are not categorized.





*/