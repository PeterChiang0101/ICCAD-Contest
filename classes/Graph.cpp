#include <bits/stdc++.h>
#include "Graph.h"
#include "Point.h"
#include "Parameter.h"

using namespace std;


vector<Point> GRAPH::Line_to_Point(const Graph Assembly) // 將線段切割成點
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

vector<vector<Point>> GRAPH::Arc_Optimization(const Graph Polygon) // 將圓弧切割成多個點，以利辨識點在圖形內外
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

Segment GRAPH::Arc_Boundary_Meas(const Segment Arc) //判段圓弧線段x,y極值
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

vector<Point> GRAPH::Arc_to_Poly(Segment Arc) //merged into "Arc_Optimization"
{
    vector<Point> Poly_out;
    Point part;

    double theta_ref;
    double theta_in;
    double theta_div;
    double radius;
    int times;
    int count;

    theta_ref = Arc.detail.theta_1;
    theta_div = 2 * PI / (360 / ARC_TO_LINE_SLICE_DENSITY); // div/degree
    radius = hypot(Arc.x1 - Arc.center_x, Arc.y1 - Arc.center_y);
    // radius = sqrt((Arc.x1 - Arc.center_x) * (Arc.x1 - Arc.center_x) + (Arc.y1 - Arc.center_y) * (Arc.y1 - Arc.center_y));

    if (Arc.is_CCW == 0) // CW
    {
        if (Arc.detail.theta_1 - Arc.detail.theta_2 <= 0)
            theta_in = Arc.detail.theta_1 - Arc.detail.theta_2 + 2 * PI;
        else
            theta_in = Arc.detail.theta_1 - Arc.detail.theta_2;
    }
    else
    {
        if (Arc.detail.theta_2 - Arc.detail.theta_1 <= 0)
            theta_in = Arc.detail.theta_2 - Arc.detail.theta_1 + 2 * PI;
        else
            theta_in = Arc.detail.theta_2 - Arc.detail.theta_1;
    }

    times = (int)(theta_in / (2 * PI) * (360 / ARC_TO_LINE_SLICE_DENSITY));
    count = times;

    while (count > 0)
    {
        if (count == times)
        {
            part.x = Arc.x1;
            part.y = Arc.y1;
        }
        else
        {
            part.x = Arc.center_x + radius * cos(theta_ref);
            part.y = Arc.center_y + radius * sin(theta_ref);
        }

        if (Arc.is_CCW == 0) // CW
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
        Poly_out.push_back(part);
        count -= 1;
    }
    if (!Poly_out.empty())
    {
        if (Poly_out.at(Poly_out.size() - 1).x != Arc.x2 && Poly_out.at(Poly_out.size() - 1).y != Arc.y2)
        {
            part.x = Arc.x2;
            part.y = Arc.y2;
            part.Next_Arc = false;
            Poly_out.push_back(part);
        }
    }

    return Poly_out;
}

Point GRAPH::intersection(const Point a1, const Point a2, const Point b1, const Point b2) //利用兩線端點,找交點
{
    Point a, b, s;
    a.x = a2.x - a1.x, a.y = a2.y - a1.y;
    b.x = b2.x - b1.x, b.y = b2.y - b1.y;
    s.x = b1.x - a1.x, s.y = b1.y - a1.y;

    // 兩線平行，交點不存在。
    // 兩線重疊，交點無限多。
    if (V_Op.cross(a, b) == 0)
    {
        s.x = s.y = INFINITY;
        return s;
    }

    // 計算交點
    Point intersect;
    intersect.x = a1.x + a.x * (V_Op.cross(s, b) / V_Op.cross(a, b));
    intersect.y = a1.y + a.y * (V_Op.cross(s, b) / V_Op.cross(a, b));

    if (point.In_Between_Lines(intersect, a1, a2) && point.In_Between_Lines(intersect, b1, b2))
        return intersect;
    else
    {
        s.x = s.y = INFINITY;
        return s;
    }
}

bool GRAPH::Line_intersect(const Segment S1, const Segment S2)//找兩Segment交點
{
    Point a1, a2, b1, b2;
    a1.x = S1.x1;
    a1.y = S1.y1;
    a2.x = S1.x2;
    a2.y = S1.y2;
    b1.x = S2.x1;
    b1.y = S2.y1;
    b2.x = S2.x2;
    b2.y = S2.y2;
    double c1 = V_Op.cross1(a1, a2, b1);
    double c2 = V_Op.cross1(a1, a2, b2);
    double c3 = V_Op.cross1(b1, b2, a1);
    double c4 = V_Op.cross1(b1, b2, a2);

    // 端點不共線
    if (c1 * c2 <= 0 && c3 * c4 <= 0)
        return true;
    // 端點共線
    /*if (c1 == 0 && Line_intersect(a1, a2, b1)) return true;
    if (c2 == 0 && Line_intersect(a1, a2, b2)) return true;
    if (c3 == 0 && Line_intersect(b1, b2, a1)) return true;
    if (c4 == 0 && Line_intersect(b1, b2, a2)) return true;*/
    return false;
}

Point GRAPH::first_intersection_between_line_and_arc_for_arc_tuning(const Segment Arc, const Point Line_First_Point, const Point Line_Second_Point)
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

    float a = V_Op.dot(d, d);
    float b = 2 * V_Op.dot(f, d);
    float c = V_Op.dot(f, f) - r * r;

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

Point GRAPH::first_intersection_between_arc_and_arc_for_arc_tuning(const Segment Arc1, const Segment Arc2)
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

vector<Point> GRAPH::intersection_between_line_and_arc(const Segment Arc, const Point Line_First_Point, const Point Line_Second_Point)
{
    // 圓公式 (x-x0)^2 + (y-y0)^2 = r^2
    // 直線公式 ax + by + c = 0
    // 交會點公式 (a^2 + b^2)x^2 + 2(-x0 * b^2 + a * c + y0 * a * b)x + ((x0^2 + y0^2 + r^2) * b^2 + c^2 - 2 * y0 * b * c) = 0
    vector<Point> Intersection_Points;

    Point d; // 直線向量
    d.x = Line_Second_Point.x - Line_First_Point.x;
    d.y = Line_Second_Point.y - Line_First_Point.y;
    Point f; // 圓至線段起點的向量
    f.x = Line_First_Point.x - Arc.center_x;
    f.y = Line_First_Point.y - Arc.center_y;
    float r = hypot(Arc.x2 - Arc.center_x, Arc.y2 - Arc.center_y); // 圓半徑

    float a = V_Op.dot(d, d);
    float b = 2 * V_Op.dot(f, d);
    float c = V_Op.dot(f, f) - r * r;

    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0)
    {
        // no intersection
        return vector<Point>();
    }
    else
    {
        // ray didn't totally miss sphere,
        // so there is a solution to
        // the equation.

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
        if (t1 >= 0 && t1 <= 1)
        {
            // t1 is the intersection, and it's closer than t2
            // (since t1 uses -b - discriminant)
            // Impale, Poke
            Point P1;
            P1.x = Line_First_Point.x + t1 * d.x;
            P1.y = Line_First_Point.y + t1 * d.y;
            float Point_Theta;
            Point_Theta = atan2(P1.y - Arc.center_y, P1.x - Arc.center_x);
            if (point.Point_Inside_Arc(Point_Theta, Arc.detail.theta_1, Arc.detail.theta_2, Arc.is_CCW))
                Intersection_Points.push_back(P1);
        }

        // here t1 didn't intersect so we are either started
        // inside the sphere or completely past it
        if (t2 >= 0 && t2 <= 1)
        {
            // ExitWound
            Point P2;
            P2.x = Line_First_Point.x + t2 * d.x;
            P2.y = Line_First_Point.y + t2 * d.y;
            float Point_Theta;
            Point_Theta = atan2(P2.y - Arc.center_y, P2.x - Arc.center_x);
            if (point.Point_Inside_Arc(Point_Theta, Arc.detail.theta_1, Arc.detail.theta_2, Arc.is_CCW))
                Intersection_Points.push_back(P2);
        }
        return Intersection_Points;
    }
    return vector<Point>();
}

vector<Point> GRAPH::intersection_between_arc_and_arc(const Segment Arc1, const Segment Arc2)
{
    float d = hypot(Arc1.center_x - Arc2.center_x, Arc1.center_y - Arc2.center_y); // 兩圓中心距離
    float r1 = hypot(Arc1.x2 - Arc1.center_x, Arc1.y2 - Arc1.center_y);            // 圓1半徑
    float r2 = hypot(Arc2.x2 - Arc2.center_x, Arc2.y2 - Arc2.center_y);            // 圓2半徑

    if (d > r1 + r2) // 兩圓相距太遠，沒有交點
        return vector<Point>();
    if (d < abs(r1 - r2)) // 兩圓相距太近，沒有交點
        return vector<Point>();
    if (d == 0 && r1 != r2) // 同心圓，沒有交點
        return vector<Point>();
    if (d == 0 && r1 == r2) // 圓完全重疊，需判斷
    {
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
        float P1_Arc1_Theta = atan2(P1.y - Arc1.center_y, P1.x - Arc1.center_x);
        float P1_Arc2_Theta = atan2(P1.y - Arc2.center_y, P1.x - Arc2.center_x);
        float P2_Arc1_Theta = atan2(P2.y - Arc1.center_y, P2.x - Arc1.center_x);
        float P2_Arc2_Theta = atan2(P2.y - Arc2.center_y, P2.x - Arc2.center_x);
        vector<Point> Intersection_Points;
        if (P1.x == P2.x && P1.y == P2.y) // 兩圓交點重疊，只有一個交點，不要回傳
        {
        }
        else
        {
            if (point.Point_Inside_Arc(P1_Arc1_Theta, Arc1.detail.theta_1, Arc1.detail.theta_2, Arc1.is_CCW) && point.Point_Inside_Arc(P1_Arc2_Theta, Arc2.detail.theta_1, Arc2.detail.theta_2, Arc2.is_CCW))
                Intersection_Points.push_back(P1);
            if (point.Point_Inside_Arc(P2_Arc1_Theta, Arc1.detail.theta_1, Arc1.detail.theta_2, Arc1.is_CCW) && point.Point_Inside_Arc(P2_Arc2_Theta, Arc2.detail.theta_1, Arc2.detail.theta_2, Arc2.is_CCW))
                Intersection_Points.push_back(P2);
            // 需判斷點是否在圓弧，用 Point_Inside_Arc 函式
        }
        return Intersection_Points;
    }
    return vector<Point>();
}