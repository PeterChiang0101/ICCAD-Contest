// Silkscreen.h
// declearation of class Silkscreen
#include "Graph.h"
#include "Segment.h"
#include "VectorOp.h"
#include "bits/stdc++.h"

#ifndef SILKSCREEN_H
#define SILKSCREEN_H

struct Intersection
{
    int copper_ID{0};      // record which copper is intersected.
    int copper_segment{0}; // the segment of intersected copper
    size_t cont_silkscreen{0};
    Point intersection_point;
};

class Silkscreen
{
public:
    Silkscreen(float); // constructor, fill the silkscreengap and gaps parameters
    vector<Graph> Silkscreen_Assembly(const Graph, const Graph, const vector<Graph>);
    //                                      assembly    silkscreen      copper
    static bool sort_increase_Segment(const Segment L1, const Segment L2);
    static bool sort_decrease_Segment(const Segment L1, const Segment L2);
    static bool sort_decrease_Arc(const Segment L1, const Segment L2);
    static bool sort_increase_Arc(const Segment L1, const Segment L2);
    static bool sort_decrease_points(const Point_ID p1, const Point_ID p2);
    static bool sort_increase_points(const Point_ID p1, const Point_ID p2);
    static bool sort_copperID(const Intersection, const Intersection);

private:
    VectorOp V_Op; // vector operator
    float silkscreenlen;
    vector<Graph> Copper_cut_segments;     // contain which copper cuts silkscreen line.
    vector<Intersection> intersect_points; // record the intersection points of the copper and the silkscreen
    Graph assembly;
    vector<Graph> silkscreen;

    //----------Untuned Silkscreen--------->main.cpp : Final_Silkscreen()
    Graph Untuned_Silkscreen(const Graph, const Graph, const vector<Graph>);
    Graph Cut_Silkscreen_by_Copper(const Segment, const vector<Graph>);
    Graph silkscreen_cut_single_copper(Segment, Graph, size_t);
    Segment Arc_Boundary_Meas_for_Assembly(const Segment);// moved to GRAPH(Arc_Boundary_Meas)
    vector<Point> intersection_between_arc_and_arc(const Segment, const Segment);// moved to GRAPH
    Graph Segment_Sort(Segment, Graph);
    vector<Point_ID> Point_Sort(const Segment, vector<Point_ID>);

    //-----------Delete_Short_Silkscreen functions----------
    void Delete_Short_Silkscreen(Graph);
    vector<Graph> Find_Continuous_Segment(Graph);
    void Find_Intersection_Copper_Silkscreen();

    //----------Fit boarder condition functions----------
    void fit_boarder_condition(Graph, Graph, vector<Graph>);
    vector<Graph> Add_Excess_Silkscreen_For_Boarder_Condition(vector<Graph>, Point, vector<Graph>, int, Graph);
    vector<vector<Point>> Arc_Optimization(Graph);//moved to GRAPH
    vector<Point> Arc_to_Poly(Segment);//moved to GRAPH
    vector<Point> Line_to_Point(const Graph);// moved to GRAPH
    bool point_in_polygon(Point, vector<Point>, vector<vector<Point>>);//moved to POINT
    float interpolate_x(float, Point, Point);//move to POINT
    bool Point_Inside_Arc(float, float, float, bool);//move to POINT
    Point intersection(Point, Point, Point, Point);//move to POINT
    bool In_Between_Lines(Point, Point, Point);//move to POINT
    vector<Point> intersection_between_line_and_arc(Segment, Point, Point);//move to GRAPH
    float Calculate_Silkscreen_length(const Graph &); // calculate the length of the Silkscreen

    int Uppest_Assembly_index;
    int Lowest_Assembly_index;
    int Leftest_Assembly_index;
    int Rightest_Assembly_index;

    array<int, 2> Uppest_Silkscreen_index;
    array<int, 2> Lowest_Silkscreen_index;
    array<int, 2> Leftest_Silkscreen_index;
    array<int, 2> Rightest_Silkscreen_index;

    void fit_lines_simularity();
    Graph cut_line_arc(Segment, const int, const bool);
    Graph cut_line(Segment, const int);
    Graph cut_arc(Segment, const int);
    // void Find_Continuous_Segment();
};

#endif // SILKSCREEN_H

// Note and issues:
/*
add sort arc tolerance
use `Calculate_Silkscreen_length` in `Delete_Short_Silkscreen`


*/