// Buffer.h
// declearation of class Buffer

#include "Graph.h"
#include "Point.h"

#ifndef BUFFER_H
#define BUFFER_H

class Buffer
{
public:
    Buffer(float, float); // 填入 gap 參數
    Graph Assembly_Buffer(const Graph);
    vector<Graph> Copper_Buffer(const vector<Graph>);

private:
    float coppergap;
    float assemblygap;
    vector<Point> Point_Extension(const Graph, const bool);                 // 圖形外擴
    vector<Point> Line_to_Point(const Graph);                               // 將線段切割成點
    vector<vector<Point>> Arc_Optimization(const Graph);                    // 將圓弧切割成多個點，以利辨識點在圖形內外
    bool point_in_polygon(Point, vector<Point>, vector<vector<Point>>);     // 運用射線法判斷點在圖形內外
    float interpolate_x(float, Point, Point);                               // 待測點與圖形邊界交會的x值
    vector<Point> Arc_Point_Tuning(const Graph, const bool, vector<Point>); // 圓與直線外擴距離不對，需用此函數修正
    Point first_intersection_between_line_and_arc_for_arc_tuning(Segment, Point, Point);
    float dot(Point, Point); // 向量積
    Point first_intersection_between_arc_and_arc_for_arc_tuning(Segment, Segment);
    Graph Point_to_Line(vector<Point>, Graph);
    Segment Arc_Boundary_Meas(Segment Arc); // 計算圓弧邊界
};

#endif // BUFFER_H