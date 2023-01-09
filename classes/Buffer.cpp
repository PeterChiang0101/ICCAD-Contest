// Buffer.cpp
// implementation of class Buffer

#include <bits/stdc++.h>
#include "Buffer.h"
#include "Point.h"
#include "Parameter.h"
#include "Graph.h"
#include "VectorOp.h"

using namespace std;

Buffer::Buffer(double assembly, double copper)
{
    assemblygap = assembly;
    coppergap = copper;
}

Graph Buffer::Assembly_Buffer(const Graph Assembly)
{
    vector<Point> Extended_Points = Point_Extension(Assembly, true);
    Graph silkscreen = Point_to_Line(Extended_Points, Assembly);
    return silkscreen;
}

vector<Graph> Buffer::Copper_Buffer(const vector<Graph> coppers)
{
    int size = coppers.size();
    Graph Single_Copper;
    vector<Point> Extended_Points_of_Single_Copper;
    vector<Graph> Every_Copper;
    for (int i = 0; i < size; i++)
    {
        Extended_Points_of_Single_Copper = Point_Extension(coppers[i], false);
        Single_Copper = Point_to_Line(Extended_Points_of_Single_Copper, coppers.at(i));
        Every_Copper.push_back(Single_Copper);
    }
    return Every_Copper;
}

vector<Point> Buffer::Point_Extension(const Graph Assembly, const bool is_assembly) // 圖形外擴
{
    const size_t size = Assembly.segment.size();
    vector<Point> Assembly_Points;
    vector<Point> Extended_Points;
    vector<vector<Point>> Arc_Dots;
    vector<Segment> Silkscreen;

    Point Failed_Point;
    Failed_Point.x = INFINITY;
    Failed_Point.y = INFINITY;

    Assembly_Points = graph_op.Line_to_Point(Assembly); //線切割為點
    Arc_Dots = graph_op.Arc_Optimization(Assembly);     // 將圓弧切割成多個點，以利辨識點在圖形內外
    if (size == 1)                                      // i think only happened in copper, eg: a full circle
    {
        Point Extended_Point;
        Extended_Point.x = Assembly.segment.at(0).x1 + coppergap * cos(Assembly.segment.at(0).detail.theta_1);
        Extended_Point.y = Assembly.segment.at(0).y1 + coppergap * sin(Assembly.segment.at(0).detail.theta_1);
        Extended_Point.Next_Arc = true;
        Extended_Points.push_back(Extended_Point);
        return Extended_Points;
    }
    for (size_t i = size - 1, j = 0; j < size; i = j++)
    {
        Segment first_line, second_line;
        double first_angle, second_angle;
        first_line = Assembly.segment.at(i);
        second_line = Assembly.segment.at(j);
        if (first_line.is_line) // first Segment is line
            first_angle = first_line.detail.theta;
        else // first Segment is arc
            first_angle = (first_line.is_CCW) ? first_line.detail.theta_2 + PI / 2 : first_line.detail.theta_2 - PI / 2;

        if (second_line.is_line) // line
            second_angle = second_line.detail.theta;
        else // arc direction 0 = ClockWise(CW), 1 = ConterClockwise(CCW)
            second_angle = (second_line.is_CCW) ? second_line.detail.theta_1 + PI / 2 : second_line.detail.theta_1 - PI / 2;

        if (Assembly_Points.at(j).x == first_line.x1 && Assembly_Points.at(j).y == first_line.y1) // 向量共同點校正
        {
            first_angle -= PI;
            if (first_angle < -PI)
                first_angle += 2 * PI;
        }
        if (Assembly_Points.at(j).x == second_line.x1 && Assembly_Points.at(j).y == second_line.y1)
        {
            second_angle -= PI;
            if (second_angle < -PI)
                second_angle += 2 * PI;
        }
        double Angle_Divided = (first_angle + second_angle) / 2; //角平分線的角度
        // float Bisector_Slope = tan(Angle_Divided);               //角平分線
        double Point_Extend_Range; //點外擴距離
        if (is_assembly)
            Point_Extend_Range = assemblygap / sin(Angle_Divided - first_angle);
        else
            Point_Extend_Range = coppergap / sin(Angle_Divided - first_angle);
        Point Extend_1, Extend_2; //交點向外延伸的兩個點
        bool Outside_1, Outside_2;
        Extend_1.x = Assembly_Points.at(j).x + Point_Extend_Range * cos(Angle_Divided);
        Extend_1.y = Assembly_Points.at(j).y + Point_Extend_Range * sin(Angle_Divided);
        Extend_2.x = Assembly_Points.at(j).x - Point_Extend_Range * cos(Angle_Divided);
        Extend_2.y = Assembly_Points.at(j).y - Point_Extend_Range * sin(Angle_Divided);

        Extend_1.Next_Arc = Assembly_Points.at(j).Next_Arc;
        Extend_2.Next_Arc = Assembly_Points.at(j).Next_Arc;

        //點是否在圖型外
        Outside_1 = !point_op.point_in_polygon(Extend_1, Assembly_Points, Arc_Dots); // true for outside, false for inside
        Outside_2 = !point_op.point_in_polygon(Extend_2, Assembly_Points, Arc_Dots);

        if (Outside_1 && !Outside_2) // 1 is outside, 2 is inside
            Extended_Points.push_back(Extend_1);
        else if (Outside_2 && !Outside_1) // 2 is outside, 1 is inside
            Extended_Points.push_back(Extend_2);
        else
            Extended_Points.push_back(Failed_Point); // 錯誤，塞入預設值
    }
    Extended_Points = Arc_Point_Tuning(Assembly, is_assembly, Extended_Points); // 圖形外擴點調整
    return Extended_Points;
}

vector<Point> Buffer::Arc_Point_Tuning(const Graph Assembly, const bool is_assembly, vector<Point> Extended_Points) // 圓與直線外擴距離不對，需用此函數修正
{
    const size_t size = Extended_Points.size();
    Segment first_line, second_line;
    float radius;
    float radius_1, radius_2;
    float radius_silkscreen;
    float radius_1_silkscreen, radius_2_silkscreen;
    bool concave; // true for concave, false for convex
    bool first_concave, second_concave;
    Segment Pushout_Circle;
    Segment Pushout_Circle_1, Pushout_Circle_2;
    Point first_point, second_point;
    Point Intersection_Points;
    double gap;

    if (is_assembly)
        gap = assemblygap;
    else
        gap = coppergap;

    for (size_t i = 0; i < size; i++)
    {
        if (i == 0)
            first_line = Assembly.segment.at(size - 1);
        else
            first_line = Assembly.segment.at(i - 1);
        second_line = Assembly.segment.at(i);
        if (Extended_Points.at(i).x == INFINITY && Extended_Points.at(i).y == INFINITY)
            continue;
        if (!first_line.is_line && second_line.is_line) // 第一個是圓弧 第二個是線段
        {
            radius = hypot(first_line.x2 - first_line.center_x, first_line.y2 - first_line.center_y);
            radius_silkscreen = hypot(Extended_Points.at(i).x - first_line.center_x, Extended_Points.at(i).y - first_line.center_y);
            if (radius_silkscreen > radius)
                concave = false; // 凸
            else
                concave = true; // 凹
            Pushout_Circle.center_x = first_line.center_x;
            Pushout_Circle.center_y = first_line.center_y;
            Pushout_Circle.is_CCW = first_line.is_CCW;
            Pushout_Circle.detail.theta_1 = first_line.detail.theta_1;
            Pushout_Circle.detail.theta_2 = first_line.detail.theta_1;

            first_point.x = Extended_Points.at(i).x;
            first_point.y = Extended_Points.at(i).y;
            if (i != size - 1)
            {
                second_point.x = Extended_Points.at(i + 1).x;
                second_point.y = Extended_Points.at(i + 1).y;
            }
            else
            {
                second_point.x = Extended_Points.at(0).x;
                second_point.y = Extended_Points.at(0).y;
            }

            if (concave)
            {
                Pushout_Circle.x1 = Pushout_Circle.x2 = first_line.center_x + (1 - gap / radius) * (first_line.x2 - first_line.center_x);
                Pushout_Circle.y1 = Pushout_Circle.y2 = first_line.center_y + (1 - gap / radius) * (first_line.y2 - first_line.center_y);
            }
            else
            {
                Pushout_Circle.x1 = Pushout_Circle.x2 = first_line.center_x + (1 + gap / radius) * (first_line.x2 - first_line.center_x);
                Pushout_Circle.y1 = Pushout_Circle.y2 = first_line.center_y + (1 + gap / radius) * (first_line.y2 - first_line.center_y);
            }

            Intersection_Points = graph_op.first_intersection_between_line_and_arc_for_arc_tuning(Pushout_Circle, first_point, second_point);

            Extended_Points.at(i).x = Intersection_Points.x;
            Extended_Points.at(i).y = Intersection_Points.y;
        }
        else if (first_line.is_line && !second_line.is_line) // 第一個是線段 第二個是圓弧
        {
            radius = hypot(second_line.x1 - second_line.center_x, second_line.y1 - second_line.center_y);
            radius_silkscreen = hypot(Extended_Points.at(i).x - second_line.center_x, Extended_Points.at(i).y - second_line.center_y);
            if (radius_silkscreen > radius)
                concave = false; // 凸
            else
                concave = true; // 凹
            Pushout_Circle.center_x = second_line.center_x;
            Pushout_Circle.center_y = second_line.center_y;
            Pushout_Circle.is_CCW = second_line.is_CCW;
            Pushout_Circle.detail.theta_1 = second_line.detail.theta_1;
            Pushout_Circle.detail.theta_2 = second_line.detail.theta_1;

            if (i != 0)
            {
                first_point.x = Extended_Points.at(i - 1).x;
                first_point.y = Extended_Points.at(i - 1).y;
            }
            else
            {
                first_point.x = Extended_Points.at(size - 1).x;
                first_point.y = Extended_Points.at(size - 1).y;
            }
            second_point.x = Extended_Points.at(i).x;
            second_point.y = Extended_Points.at(i).y;

            if (concave)
            {
                Pushout_Circle.x1 = Pushout_Circle.x2 = second_line.center_x + (1 - gap / radius) * (second_line.x1 - second_line.center_x);
                Pushout_Circle.y1 = Pushout_Circle.y2 = second_line.center_y + (1 - gap / radius) * (second_line.y1 - second_line.center_y);
            }
            else
            {
                Pushout_Circle.x1 = Pushout_Circle.x2 = second_line.center_x + (1 + gap / radius) * (second_line.x1 - second_line.center_x);
                Pushout_Circle.y1 = Pushout_Circle.y2 = second_line.center_y + (1 + gap / radius) * (second_line.y1 - second_line.center_y);
            }

            Intersection_Points = graph_op.first_intersection_between_line_and_arc_for_arc_tuning(Pushout_Circle, second_point, first_point);

            Extended_Points.at(i).x = Intersection_Points.x;
            Extended_Points.at(i).y = Intersection_Points.y;
        }
        else if (!first_line.is_line && !second_line.is_line)
        {
            radius_1 = hypot(first_line.x2 - first_line.center_x, first_line.y2 - first_line.center_y);
            radius_2 = hypot(second_line.x1 - second_line.center_x, second_line.y1 - second_line.center_y);
            radius_1_silkscreen = hypot(Extended_Points.at(i).x - first_line.center_x, Extended_Points.at(i).y - first_line.center_y);
            radius_2_silkscreen = hypot(Extended_Points.at(i).x - second_line.center_x, Extended_Points.at(i).y - second_line.center_y);
            if (radius_1_silkscreen > radius_1)
                first_concave = false; // 凸
            else
                first_concave = true; // 凹
            Pushout_Circle_1.center_x = first_line.center_x;
            Pushout_Circle_1.center_y = first_line.center_y;
            Pushout_Circle_1.is_CCW = first_line.is_CCW;
            Pushout_Circle_1.detail.theta_1 = first_line.detail.theta_1;
            Pushout_Circle_1.detail.theta_2 = first_line.detail.theta_1;
            if (first_concave)
            {
                Pushout_Circle_1.x1 = Pushout_Circle_1.x2 = first_line.center_x + (1 - gap / radius_1) * (first_line.x2 - first_line.center_x);
                Pushout_Circle_1.y1 = Pushout_Circle_1.y2 = first_line.center_y + (1 - gap / radius_1) * (first_line.y2 - first_line.center_y);
            }
            else
            {
                Pushout_Circle_1.x1 = Pushout_Circle_1.x2 = first_line.center_x + (1 + gap / radius_1) * (first_line.x2 - first_line.center_x);
                Pushout_Circle_1.y1 = Pushout_Circle_1.y2 = first_line.center_y + (1 + gap / radius_1) * (first_line.y2 - first_line.center_y);
            }

            if (radius_2_silkscreen > radius_2)
                second_concave = false; // 凸
            else
                second_concave = true; // 凹
            Pushout_Circle_2.center_x = second_line.center_x;
            Pushout_Circle_2.center_y = second_line.center_y;
            Pushout_Circle_2.is_CCW = second_line.is_CCW;
            Pushout_Circle_2.detail.theta_1 = second_line.detail.theta_1;
            Pushout_Circle_2.detail.theta_2 = second_line.detail.theta_1;

            if (second_concave)
            {
                Pushout_Circle_2.x1 = Pushout_Circle_2.x2 = second_line.center_x + (1 - gap / radius_2) * (second_line.x1 - second_line.center_x);
                Pushout_Circle_2.y1 = Pushout_Circle_2.y2 = second_line.center_y + (1 - gap / radius_2) * (second_line.y1 - second_line.center_y);
            }
            else
            {
                Pushout_Circle_2.x1 = Pushout_Circle_2.x2 = second_line.center_x + (1 + gap / radius_2) * (second_line.x1 - second_line.center_x);
                Pushout_Circle_2.y1 = Pushout_Circle_2.y2 = second_line.center_y + (1 + gap / radius_2) * (second_line.y1 - second_line.center_y);
            }

            Intersection_Points = graph_op.first_intersection_between_arc_and_arc_for_arc_tuning(Pushout_Circle_1, Pushout_Circle_2);
            Extended_Points.at(i).x = Intersection_Points.x;
            Extended_Points.at(i).y = Intersection_Points.y;
        }
    }
    return Extended_Points;
}

Graph Buffer::Point_to_Line(vector<Point> Extended_Points, Graph Polygon)
{
    size_t size = Polygon.segment.size();
    Segment A_Line;
    Graph Silkscreen;
    if (!Extended_Points.empty())
    {
        Silkscreen.x_min = Silkscreen.x_max = Extended_Points.at(0).x; // initialize
        Silkscreen.y_min = Silkscreen.y_max = Extended_Points.at(0).y;
        for (size_t i = 0; i < size; i++)
        {
            // calculate point to line
            A_Line.is_line = (Extended_Points.at(i).Next_Arc) ? false : true;
            A_Line.x1 = Extended_Points.at(i).x;
            A_Line.y1 = Extended_Points.at(i).y;
            if (i != size - 1)
            {
                A_Line.x2 = Extended_Points.at(i + 1).x;
                A_Line.y2 = Extended_Points.at(i + 1).y;
            }
            else
            {
                A_Line.x2 = Extended_Points.at(0).x;
                A_Line.y2 = Extended_Points.at(0).y;
            }
            if (A_Line.is_line)
            {
                A_Line.center_x = A_Line.center_y = A_Line.is_CCW = 0;
            }
            else
            {
                A_Line.detail.theta = 0;

                A_Line.center_x = Polygon.segment.at(i).center_x;
                A_Line.center_y = Polygon.segment.at(i).center_y;
                A_Line.is_CCW = Polygon.segment.at(i).is_CCW;

                A_Line.detail.theta_1 = atan2(A_Line.y1 - A_Line.center_y, A_Line.x1 - A_Line.center_x);
                A_Line.detail.theta_2 = atan2(A_Line.y2 - A_Line.center_y, A_Line.x2 - A_Line.center_x);
            }
            if (!A_Line.is_line) // arc
            {
                A_Line = graph_op.Arc_Boundary_Meas(A_Line);
            }
            else // line
            {
                A_Line.detail.x_min = min(A_Line.x1, A_Line.x2);
                A_Line.detail.x_max = max(A_Line.x1, A_Line.x2);
                A_Line.detail.y_min = min(A_Line.y1, A_Line.y2);
                A_Line.detail.y_max = max(A_Line.y1, A_Line.y2);
            }

            if (A_Line.detail.x_max > Silkscreen.x_max)
            {
                Silkscreen.x_max = A_Line.detail.x_max;
            }
            if (A_Line.detail.x_min < Silkscreen.x_min)
            {
                Silkscreen.x_min = A_Line.detail.x_min;
            }
            if (A_Line.detail.y_max > Silkscreen.y_max)
            {
                Silkscreen.y_max = A_Line.detail.y_max;
            }
            if (A_Line.detail.y_min < Silkscreen.y_min)
            {
                Silkscreen.y_min = A_Line.detail.y_min;
            }

            Silkscreen.segment.push_back(A_Line);
        }
    }

    return Silkscreen;
}

//-----------removed functions---------
/*
vector<Point> Buffer::Line_to_Point(const Graph Assembly) // 將線段切割成點  // moved to GRAPH
{
    const size_t size = Assembly.segment.size();
    vector<Point> Point_Vector;
    Segment first_line, second_line;
    Point Point_Overlap; //兩線段交點

    for (size_t i = size - 1, j = 0; j < size; i = j++)
    {
        first_line = Assembly.segment.at(i);
        second_line = Assembly.segment.at(j);
        if ((first_line.x1 == second_line.x1 && first_line.y1 == second_line.y1) || (first_line.x1 == second_line.x2 && first_line.y1 == second_line.y2)) //找重疊線段
        {
            Point_Overlap.x = first_line.x1;
            Point_Overlap.y = first_line.y1;
        }
        else
        {
            Point_Overlap.x = first_line.x2;
            Point_Overlap.y = first_line.y2;
        }
        if (second_line.is_line)
            Point_Overlap.Next_Arc = false;
        else
            Point_Overlap.Next_Arc = true;
        Point_Vector.push_back(Point_Overlap);
    }
    return Point_Vector;
}

vector<vector<Point>> Buffer::Arc_Optimization(const Graph Polygon) // 將圓弧切割成多個點，以利辨識點在圖形內外 //moved to GRAPH
{
    int Assembly_size = Polygon.segment.size();
    vector<Point> Dots_of_Arc;
    vector<vector<Point>> vector_of_Arc;
    Segment Single_line;
    for (int i = 0; i < Assembly_size; i++)
    {
        Single_line = Polygon.segment.at(i);
        if (!Single_line.is_line) // the Segment is arc
        {
            Dots_of_Arc.clear();
            vector<Point> Dots_of_Arc;
            Point part;

            double theta_ref;
            double theta_in;
            double theta_div;
            double radius;
            int times;
            int count;

            theta_ref = Single_line.detail.theta_1;
            theta_div = 2 * PI / (360 / ARC_TO_LINE_SLICE_DENSITY); // div/degree
            radius = hypot(Single_line.x1 - Single_line.center_x, Single_line.y1 - Single_line.center_y);
            // radius = sqrt((Single_line.x1 - Single_line.center_x) * (Single_line.x1 - Single_line.center_x) + (Single_line.y1 - Single_line.center_y) * (Single_line.y1 - Single_line.center_y));

            if (Single_line.is_CCW == 0) // CW
            {
                if (Single_line.detail.theta_1 - Single_line.detail.theta_2 <= 0)
                    theta_in = Single_line.detail.theta_1 - Single_line.detail.theta_2 + 2 * PI;
                else
                    theta_in = Single_line.detail.theta_1 - Single_line.detail.theta_2;
            }
            else
            {
                if (Single_line.detail.theta_2 - Single_line.detail.theta_1 <= 0)
                    theta_in = Single_line.detail.theta_2 - Single_line.detail.theta_1 + 2 * PI;
                else
                    theta_in = Single_line.detail.theta_2 - Single_line.detail.theta_1;
            }

            times = (int)(theta_in / (2 * PI) * (360 / ARC_TO_LINE_SLICE_DENSITY));
            count = times;

            while (count > 0)
            {
                if (count == times)
                {
                    part.x = Single_line.x1;
                    part.y = Single_line.y1;
                }
                else
                {
                    part.x = Single_line.center_x + radius * cos(theta_ref);
                    part.y = Single_line.center_y + radius * sin(theta_ref);
                }

                if (Single_line.is_CCW == 0) // CW
                {
                    theta_ref -= theta_div;
                    if (theta_ref < -PI)
                        theta_ref += 2 * PI;
                }
                else
                {
                    theta_ref += theta_div;
                    if (theta_ref > PI)
                        theta_ref -= 2 * PI;
                }
                part.Next_Arc = false;
                Dots_of_Arc.push_back(part);
                count -= 1;
            }
            if (!Dots_of_Arc.empty())
            {
                if (Dots_of_Arc.at(Dots_of_Arc.size() - 1).x != Single_line.x2 && Dots_of_Arc.at(Dots_of_Arc.size() - 1).y != Single_line.y2)
                {
                    part.x = Single_line.x2;
                    part.y = Single_line.y2;
                    part.Next_Arc = false;
                    Dots_of_Arc.push_back(part);
                }
            }
            vector_of_Arc.push_back(Dots_of_Arc);
        }
    }
    return vector_of_Arc;
}

bool Buffer::point_in_polygon(Point t, vector<Point> Assembly_Point, vector<vector<Point>> Arc_Points) // 運用射線法判斷點在圖形內外 //moved into POINT
{
    int Assembly_size = Assembly_Point.size();
    int Arc_count = 0;
    bool c = false;
    for (int i = Assembly_size - 1, j = 0; j < Assembly_size; i = j++)
    {
        if (Assembly_Point.at(i).Next_Arc)
        {
            int Arc_point_length = Arc_Points.at(Arc_count).size();
            for (int k = 0, l = 1; l < Arc_point_length; k = l++)
            {
                if ((Arc_Points.at(Arc_count).at(k).y > t.y) != (Arc_Points.at(Arc_count).at(l).y > t.y) && t.x < interpolate_x(t.y, Arc_Points.at(Arc_count).at(k), Arc_Points.at(Arc_count).at(l)))
                    c = !c;
            }
            Arc_count++;
        }
        else
            // 待測點在該線段的高度上下限內 且 交會點x值大於待測點x值 (射線為 + x 方向)
            if ((Assembly_Point.at(i).y > t.y) != (Assembly_Point.at(j).y > t.y) && t.x < interpolate_x(t.y, Assembly_Point.at(i), Assembly_Point.at(j)))
                c = !c;
    }
    return c;
}

float Buffer::interpolate_x(float y, Point p1, Point p2) // 待測點與圖形邊界交會的x值 // move to point.cpp
{
    if (p1.y == p2.y)
        return p1.x;
    return p1.x + (p2.x - p1.x) * (y - p1.y) / (p2.y - p1.y);
}
*/
/*
Point Buffer::first_intersection_between_line_and_arc_for_arc_tuning(Segment Arc, Point Line_First_Point, Point Line_Second_Point)
{
    // 圓公式 (x-x0)^2 + (y-y0)^2 = r^2
    // 直線公式 ax + by + c = 0
    // 交會點公式 (a^2 + b^2)x^2 + 2(-x0 * b^2 + a * c + y0 * a * b)x + ((x0^2 + y0^2 + r^2) * b^2 + c^2 - 2 * y0 * b * c) = 0

    Point d; // 直線向量
    d.x = Line_Second_Point.x - Line_First_Point.x;
    d.y = Line_Second_Point.y - Line_First_Point.y;
    Point f; // 圓至線段起點的向量
    f.x = Line_First_Point.x - Arc.center_x;
    f.y = Line_First_Point.y - Arc.center_y;
    float r = hypot(Arc.x2 - Arc.center_x, Arc.y2 - Arc.center_y); // 圓半徑

    float a = dot(d, d);
    float b = 2 * dot(f, d);
    float c = dot(f, f) - r * r;

    float discriminant = b * b - 4 * a * c;
    if (discriminant + Subtraction_Tolerance < 0)
    {
        // no intersection
        return Point();
    }
    else
    {
        // ray didn't totally miss sphere,
        // so there is a solution to
        // the equation.
        if (discriminant < 0)
        {
            discriminant = 0;
        }

        discriminant = sqrt(discriminant);

        // either solution may be on or off the ray so need to test both
        // t1 is always the smaller value, because BOTH discriminant and
        // a are nonnegative.
        float t1 = (-b - discriminant) / (2 * a); // 方程式的兩個解
        float t2 = (-b + discriminant) / (2 * a);

        // 3x HIT cases:
        //          -o->             --|-->  |            |  --|->
        // Impale(t1 hit,t2 hit), Poke(t1 hit,t2>1), ExitWound(t1<0, t2 hit),

        // 3x MISS cases:
        //       ->  o                     o ->              | -> |
        // FallShort (t1>1,t2>1), Past (t1<0,t2<0), CompletelyInside(t1<0, t2>1)

        // P = E + t * d

        Point P1;
        P1.x = Line_First_Point.x + t1 * d.x;
        P1.y = Line_First_Point.y + t1 * d.y;

        Point P2;
        P2.x = Line_First_Point.x + t2 * d.x;
        P2.y = Line_First_Point.y + t2 * d.y;

        return (min(abs(t1), abs(t2)) == t1) ? P1 : P2;
    }
    return Point();
}

float Buffer::dot(Point v1, Point v2) // 向量積 //removed ,replaced by vectorop
{
    return v1.x * v2.x + v1.y * v2.y;
}

Point Buffer::first_intersection_between_arc_and_arc_for_arc_tuning(Segment Arc1, Segment Arc2) //inputoupt.cpp
{
    float d = hypot(Arc1.center_x - Arc2.center_x, Arc1.center_y - Arc2.center_y); // 兩圓中心距離
    float r1 = hypot(Arc1.x2 - Arc1.center_x, Arc1.y2 - Arc1.center_y);            // 圓1半徑
    float r2 = hypot(Arc2.x2 - Arc2.center_x, Arc2.y2 - Arc2.center_y);            // 圓2半徑

    if (d > r1 + r2) // 兩圓相距太遠，沒有交點
        return Point();
    if (d < abs(r1 - r2)) // 兩圓相距太近，沒有交點
        return Point();
    if (d == 0 && r1 != r2) // 同心圓，沒有交點
        return Point();
    if (d == 0 && r1 == r2) // 圓完全重疊，需判斷
    {
        Point P;
        P.x = Arc1.x2;
        P.y = Arc1.y2;
        P.Next_Arc = true;
        return P;
    }
    else
    {
        float a = (r1 * r1 - r2 * r2 + d * d) / (2 * d);
        float h = sqrt(r1 * r1 - a * a);
        float x0 = Arc1.center_x + a * (Arc2.center_x - Arc1.center_x) / d;
        float y0 = Arc1.center_y + a * (Arc2.center_y - Arc1.center_y) / d;
        float x1 = x0 + h * (Arc2.center_y - Arc1.center_y) / d;
        float y1 = y0 - h * (Arc2.center_x - Arc1.center_x) / d;
        float x2 = x0 - h * (Arc2.center_y - Arc1.center_y) / d;
        float y2 = y0 + h * (Arc2.center_x - Arc1.center_x) / d;
        Point P1;
        P1.x = x1;
        P1.y = y1;
        Point P2;
        P2.x = x2;
        P2.y = y2;
        float P1_distance = hypot(P1.x - Arc1.x2, P1.y - Arc1.x2);
        float P2_distance = hypot(P2.x - Arc1.x2, P2.y - Arc1.x2);
        return (abs(P1_distance) < abs(P2_distance)) ? P1 : P2;
    }
    return Point();
}

Segment Buffer::Arc_Boundary_Meas(Segment Arc) // move to graph.cpp
{
    Segment A_Arc;
    A_Arc = Arc;
    float first_angle, second_angle;
    float radius = hypot(Arc.x1 - Arc.center_x, Arc.y1 - Arc.center_y);
    first_angle = A_Arc.detail.theta_1;
    second_angle = A_Arc.detail.theta_2;
    if (first_angle == second_angle)
    {
        A_Arc.detail.x_min = Arc.center_x - radius;
        A_Arc.detail.x_max = Arc.center_x + radius;
        A_Arc.detail.y_min = Arc.center_y - radius;
        A_Arc.detail.y_max = Arc.center_y + radius;
        return A_Arc;
    }

    if (Arc.is_CCW)
    {
        swap(first_angle, second_angle);
    }
    if (first_angle > second_angle)
    {
        A_Arc.detail.x_max = (first_angle >= 0 && second_angle <= 0) ? Arc.center_x + radius : max(max(Arc.x1, Arc.x2), Arc.center_x);
        A_Arc.detail.x_min = min(min(Arc.x1, Arc.x2), Arc.center_x);
        A_Arc.detail.y_max = (first_angle >= PI / 2 && second_angle <= PI / 2) ? Arc.center_y + radius : max(max(Arc.y1, Arc.y2), Arc.center_y);
        A_Arc.detail.y_min = (first_angle >= -PI / 2 && second_angle <= -PI / 2) ? Arc.center_y - radius : min(min(Arc.y1, Arc.y2), Arc.center_y);
    }
    else if (second_angle < 0) // first < second < 0
    {
        A_Arc.detail.x_max = Arc.center_x + radius;
        A_Arc.detail.x_min = Arc.center_x - radius;
        A_Arc.detail.y_max = Arc.center_y + radius;
        A_Arc.detail.y_min = (first_angle >= -PI / 2 || second_angle <= -PI / 2) ? Arc.center_y - radius : min(min(Arc.y1, Arc.y2), Arc.center_y);
    }
    else if (first_angle >= 0) // 0 <= first < second
    {
        A_Arc.detail.x_max = Arc.center_x + radius;
        A_Arc.detail.x_min = Arc.center_x - radius;
        A_Arc.detail.y_max = (first_angle >= PI / 2 || second_angle <= PI / 2) ? Arc.center_y + radius : max(max(Arc.y1, Arc.y2), Arc.center_y);
        A_Arc.detail.y_min = Arc.center_y - radius;
    }
    else // first < 0 < second
    {
        A_Arc.detail.x_max = max(max(Arc.x1, Arc.x2), Arc.center_x);
        A_Arc.detail.x_min = Arc.center_x - radius;
        A_Arc.detail.y_max = (second_angle <= PI / 2) ? Arc.center_y + radius : max(max(Arc.y1, Arc.y2), Arc.center_y);
        A_Arc.detail.y_min = (first_angle >= -PI / 2) ? Arc.center_y - radius : min(min(Arc.y1, Arc.y2), Arc.center_y);
    }

    return A_Arc;
}

*/