// Silkscreen.h
// declearation of class Silkscreen
#include "Graph.h"
#include "Segment.h"
#include "VectorOp.h"

#ifndef SILKSCREEN_H
#define SILKSCREEN_H

struct Intersection
{
    int copper_ID{0}; //record which copper is intersected.
    int copper_segment{0};// the segment of intersected copper
    size_t cont_silkscreen{0};
    Point intersection_point;
};

class Silkscreen
{
public:
    Silkscreen(float); //constructor, fill the silkscreengap and gaps parameters
    vector<Graph> Silkscreen_Assembly(const Graph, const Graph, const vector<Graph>);
    //                                      assembly    silkscreen      copper

private:
    VectorOp V_Op; // vector operator
    float coppergap;
    //float assemblygap; ?
    float silkscreenlen;
    vector<Graph> Copper_cut_segments;//contain which copper cuts silkscreen line.
    vector<Intersection> intersect_points;//record the intersection points of the copper and the silkscreen
    //----------Untuned Silkscreen--------->main.cpp : Final_Silkscreen()
    Graph Untuned_Silkscreen(const Graph, const vector<Graph>);
    Graph Cut_Silkscreen_by_Copper(const Segment, const vector<Graph>);
    Graph silkscreen_cut_single_copper(Segment, Graph);
    Segment Arc_Boundary_Meas_for_Assembly(const Segment);
    vector<Point> intersection_between_arc_and_arc(const Segment, const Segment);
    Graph Segment_Sort(Segment, Graph);
    bool sort_increase_Segment(const Segment, const Segment);
    bool sort_decrease_Segment(const Segment, const Segment);
    bool sort_decrease_Arc(const Segment, const Segment);
    bool sort_increase_Arc(const Segment, const Segment);
    vector<Point_ID> Point_Sort(const Segment, vector<Point_ID>);
    bool sort_decrease_points(const Point_ID, const Point_ID);
    bool sort_increase_points(const Point_ID, const Point_ID);

    //-----------Delete_Short_Silkscreen functions----------
    vector<Graph> Delete_Short_Silkscreen(Graph);
    vector<Graph> Find_Continuous_Segment(Graph);
    void Find_Intersection_Copper_Silkscreen(const vector<Graph>);

    //----------Fit boarder condition functions----------
    vector<Graph> fit_boarder_condition(vector<Graph>, Graph, Graph, vector<Graph>);
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
add sort arc tolerance
use `Calculate_Silkscreen_length` in `Delete_Short_Silkscreen`
silkscreen_cut_single_copper 需要用到“Graph"資料型態 (用Segment)


*/