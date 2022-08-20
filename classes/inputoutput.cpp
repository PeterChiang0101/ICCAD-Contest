// function of inputoutput.h // this cpp is going to be removed
#include <bits/stdc++.h>
#include "inputoutput.h"

using namespace std;

#define PI 3.14159265358979323846
#define ARC_TO_LINE_SLICE_DENSITY 1 // 切片密度(in degree)

////////////////Function Declaration/////////////
float interpolate_x(float, Point, Point);
Copper Arc_Boundary_Meas(Segment);
Segment Arc_Boundary_Meas_for_Assembly(Segment);
vector<Point> Arc_Point_Tuning(const vector<Segment>, const bool, vector<Point>, float, float);
Point first_intersection_between_line_and_arc_for_arc_tuning(Segment, Point, Point);
Point first_intersection_between_arc_and_arc_for_arc_tuning(Segment, Segment);
float Dot(Point, Point);
/////////////////End Function Declaration////////
/*
float Input_Output::File_to_Parameter(const string str) // 讀入參數 //OK
{
    string str_truncate;
    str_truncate = str.substr(str.find(',') + 1);
    return stof(str_truncate);
}

const vector<string> Input_Output::split(const string &str, const char &delimiter) // 拆分文字 //OK
{
    vector<string> result;
    stringstream ss(str);
    string tok;

    while (getline(ss, tok, delimiter))
    {
        result.push_back(tok);
    }
    return result;
}

vector<Segment> Input_Output::Read_Assembly(fstream &Input_File) // 讀取assembly，轉換為vector //OK
{
    vector<Segment> Assembly;
    Segment part;
    vector<string> split_return;
    string line;
    getline(Input_File, line);

    while (getline(Input_File, line))
    {
        if (line == "copper")
            return Assembly;
        else if (line == "assembly")
            continue;
        else
            part = String_to_Line(line);
        Assembly.push_back(part);
    }
    return Assembly;
}

vector<vector<Segment>> Input_Output::Read_Copper(fstream &Input_File) // 讀取copper，轉換為二維vector //OK
{
    vector<Segment> copper;
    vector<vector<Segment>> copper_pack;
    Segment part;
    vector<string> split_return;
    string line;
    while (getline(Input_File, line))
    {
        if (line == "copper")
        {
            copper_pack.push_back(copper);
            copper.clear();
        }
        else
        {
            part = String_to_Line(line);
            copper.push_back(part);
        }
    }
    copper_pack.push_back(copper);
    return copper_pack;
}

Segment Input_Output::String_to_Line(string line) // 讀取時建立線段 
{
    vector<string> Splited;
    Splited = split(line, ',');
    size_t vector_size = Splited.size();
    Segment part;

    for (size_t i = 1; i < vector_size; i++)
    {
        switch (i)
        {
        case 1:
            part.x1 = stof(Splited.at(i));
            break;
        case 2:
            part.y1 = stof(Splited.at(i));
            break;
        case 3:
            part.x2 = stof(Splited.at(i));
            break;
        case 4:
            part.y2 = stof(Splited.at(i));
            break;
        case 5:
            part.center_x = stof(Splited.at(i));
            break;
        case 6:
            part.center_y = stof(Splited.at(i));
            break;
        case 7:
            part.direction = (Splited.at(i) == "CCW") ? 1 : 0;
            break;
        }
    }
    if (vector_size == 5)
    {
        part.is_line = true;
        part.center_x = 0;
        part.center_y = 0;
        part.direction = 0;
        part.slope = (part.y2 - part.y1) / (part.x2 - part.x1);
        part.y_intercept = part.y1 - part.slope * part.x1;
        part.theta = atan2(part.y2 - part.y1, part.x2 - part.x1);
        part.theta_1 = 0;
        part.theta_2 = 0;
    }
    else if (vector_size == 8)
    {
        part.is_line = false;
        part.slope = 0;
        part.y_intercept = 0;
        part.theta = 0;
        part.theta_1 = atan2(part.y1 - part.center_y, part.x1 - part.center_x);
        part.theta_2 = atan2(part.y2 - part.center_y, part.x2 - part.center_x);
    }
    return part;
}

vector<Segment> Input_Output::Assembly_Buffer(const vector<Segment> Assembly, float coppergap, float assemblygap) // in buffer.cpp public 
{
    vector<Point> Extended_Points = Point_Extension(Assembly, true, coppergap, assemblygap);
    vector<Segment> silkscreen = Point_to_Line(Extended_Points, Assembly);
    return silkscreen;
}

vector<Point> Input_Output::Point_Extension(const vector<Segment> Assembly, const bool is_assembly, float coppergap, float assemblygap) // 圖形外擴 //buffer.cpp private
{
    const size_t size = Assembly.size();
    vector<Point> Assembly_Points;
    vector<Point> Extended_Points;
    vector<vector<Point>> Arc_Dots;
    vector<Segment> Silkscreen;

    Point Failed_Point;
    Failed_Point.x = INFINITY;
    Failed_Point.y = INFINITY;

    Assembly_Points = Line_to_Point(Assembly); //線切割為點
    Arc_Dots = Arc_Optimization(Assembly);     // 將圓弧切割成多個點，以利辨識點在圖形內外
    if (size == 1)                             // i think only happened in copper, eg: a full circle
    {
        Point Extended_Point;
        Extended_Point.x = Assembly.at(0).x1 + coppergap * cos(Assembly.at(0).theta_1);
        Extended_Point.y = Assembly.at(0).y1 + coppergap * sin(Assembly.at(0).theta_1);
        Extended_Point.Next_Arc = true;
        Extended_Points.push_back(Extended_Point);
        return Extended_Points;
    }
    for (size_t i = size - 1, j = 0; j < size; i = j++)
    {
        Segment first_line, second_line;
        double first_angle, second_angle;
        first_line = Assembly.at(i);
        second_line = Assembly.at(j);
        if (first_line.is_line) // first Segment is line
            first_angle = first_line.theta;
        else // first Segment is arc
            first_angle = (first_line.direction) ? first_line.theta_2 + PI / 2 : first_line.theta_2 - PI / 2;

        if (second_line.is_line) // line
            second_angle = second_line.theta;
        else // arc direction 0 = ClockWise(CW), 1 = ConterClockwise(CCW)
            second_angle = (second_line.direction) ? second_line.theta_1 + PI / 2 : second_line.theta_1 - PI / 2;

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
        Outside_1 = !point_in_polygon(Extend_1, Assembly_Points, Arc_Dots); // true for outside, false for inside
        Outside_2 = !point_in_polygon(Extend_2, Assembly_Points, Arc_Dots);

        if (Outside_1 && !Outside_2) // 1 is outside, 2 is inside
            Extended_Points.push_back(Extend_1);
        else if (Outside_2 && !Outside_1) // 2 is outside, 1 is inside
            Extended_Points.push_back(Extend_2);
        else
            Extended_Points.push_back(Failed_Point);
    }
    Extended_Points = Arc_Point_Tuning(Assembly, is_assembly, Extended_Points, coppergap, assemblygap); // 圖形外擴點調整
    return Extended_Points;
}

vector<Point> Input_Output::Arc_Point_Tuning(const vector<Segment> Assembly, const bool is_assembly, vector<Point> Extended_Points, float coppergap, float assemblygap) // 圓與直線外擴距離不對，需用此函數修正 // in buffer.cpp private
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
    float gap;
    if (is_assembly)
        gap = assemblygap;
    else
        gap = coppergap;

    for (size_t i = 0; i < size; i++)
    {
        if (i == 0)
            first_line = Assembly.at(size - 1);
        else
            first_line = Assembly.at(i - 1);
        second_line = Assembly.at(i);

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
            Pushout_Circle.direction = first_line.direction;
            Pushout_Circle.theta_1 = first_line.theta_1;
            Pushout_Circle.theta_2 = first_line.theta_1;

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

            Intersection_Points = first_intersection_between_line_and_arc_for_arc_tuning(Pushout_Circle, first_point, second_point);

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
            Pushout_Circle.direction = second_line.direction;
            Pushout_Circle.theta_1 = second_line.theta_1;
            Pushout_Circle.theta_2 = second_line.theta_1;

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

            Intersection_Points = first_intersection_between_line_and_arc_for_arc_tuning(Pushout_Circle, second_point, first_point);

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
            Pushout_Circle_1.direction = first_line.direction;
            Pushout_Circle_1.theta_1 = first_line.theta_1;
            Pushout_Circle_1.theta_2 = first_line.theta_1;
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
            Pushout_Circle_2.direction = second_line.direction;
            Pushout_Circle_2.theta_1 = second_line.theta_1;
            Pushout_Circle_2.theta_2 = second_line.theta_1;

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

            Intersection_Points = first_intersection_between_arc_and_arc_for_arc_tuning(Pushout_Circle_1, Pushout_Circle_2);
            Extended_Points.at(i).x = Intersection_Points.x;
            Extended_Points.at(i).y = Intersection_Points.y;
        }
    }
    return Extended_Points;
}

*/
Point Input_Output::first_intersection_between_line_and_arc_for_arc_tuning(Segment Arc, Point Line_First_Point, Point Line_Second_Point) //moved to GRAPH
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

    float a = Dot(d, d);
    float b = 2 * Dot(f, d);
    float c = Dot(f, f) - r * r;

    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0)
    {
        // no intersection
        return Point();
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

Point Input_Output::first_intersection_between_arc_and_arc_for_arc_tuning(Segment Arc1, Segment Arc2) //moved to GRAPH
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

float Input_Output::Dot(Point v1, Point v2) // 向量積 // vectorop
{
    return v1.x * v2.x + v1.y * v2.y;
}

/************************* redefinition of the same function************************
bool Input_Output::point_in_polygon(Point t, vector<Point> Assembly_Point, vector<vector<Point>> Arc_Points) // 運用射線法判斷點在圖形內外
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
*/

float interpolate_x(float y, Point p1, Point p2) // 待測點與圖形邊界交會的x值 //move to point.cpp
{
    if (p1.y == p2.y)
        return p1.x;
    return p1.x + (p2.x - p1.x) * (y - p1.y) / (p2.y - p1.y);
}

vector<Point> Input_Output::Line_to_Point(const vector<Segment> Assembly) // 將線段切割成點 //move to graph.cpp
{
    const size_t size = Assembly.size();
    vector<Point> Point_Vector;
    Segment first_line, second_line;
    Point Point_Overlap; //兩線段交點

    for (size_t i = size - 1, j = 0; j < size; i = j++)
    {
        first_line = Assembly.at(i);
        second_line = Assembly.at(j);
        if ((first_line.x1 == second_line.x1 && first_line.y1 == second_line.y1) || (first_line.x1 == second_line.x2 && first_line.y1 == second_line.y2)) //找重疊線段
        {
            Point_Overlap.x = first_line.x1;
            Point_Overlap.y = first_line.y1;
            // if(!first_line.is_line) Point_Overlap.arc_or_not = true;
        }
        else
        {
            Point_Overlap.x = first_line.x2;
            Point_Overlap.y = first_line.y2;
            // if(!second_line.is_line) Point_Overlap.arc_or_not = true;
        }
        if (second_line.is_line)
            Point_Overlap.Next_Arc = false;
        else
            Point_Overlap.Next_Arc = true;
        Point_Vector.push_back(Point_Overlap);
    }
    return Point_Vector;
}

vector<vector<Point>> Input_Output::Arc_Optimization(vector<Segment> Assembly)//moved to GRAPH
{
    int Assembly_size = Assembly.size();
    vector<Point> Dots_of_Arc;
    vector<vector<Point>> vector_of_Arc;
    for (int i = 0; i < Assembly_size; i++)
    {
        if (!Assembly.at(i).is_line) // the Segment is arc
        {
            Dots_of_Arc.clear();
            Dots_of_Arc = Arc_to_Poly(Assembly.at(i));
            vector_of_Arc.push_back(Dots_of_Arc);
        }
    }
    return vector_of_Arc;
}

vector<Point> Input_Output::Arc_to_Poly(Segment Arc)//merged into "Arc_Optimization"
{
    vector<Point> Poly_out;
    Point part;

    double theta_ref;
    double theta_in;
    double theta_div;
    double radius;
    int times;
    int count;

    theta_ref = Arc.theta_1;
    theta_div = 2 * PI / (360 / ARC_TO_LINE_SLICE_DENSITY); // div/degree
    radius = hypot(Arc.x1 - Arc.center_x, Arc.y1 - Arc.center_y);
    // radius = sqrt((Arc.x1 - Arc.center_x) * (Arc.x1 - Arc.center_x) + (Arc.y1 - Arc.center_y) * (Arc.y1 - Arc.center_y));

    if (Arc.direction == 0) // CW
    {
        if (Arc.theta_1 - Arc.theta_2 <= 0)
            theta_in = Arc.theta_1 - Arc.theta_2 + 2 * PI;
        else
            theta_in = Arc.theta_1 - Arc.theta_2;
    }
    else
    {
        if (Arc.theta_2 - Arc.theta_1 <= 0)
            theta_in = Arc.theta_2 - Arc.theta_1 + 2 * PI;
        else
            theta_in = Arc.theta_2 - Arc.theta_1;
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

        if (Arc.direction == 0) // CW
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

vector<Segment> Input_Output::Point_to_Line(vector<Point> Extended_Points, vector<Segment> Assembly) // assembly 專屬 //in buffer.cpp private
{
    size_t size = Assembly.size();
    Segment A_Line;
    vector<Segment> Silkscreen;
    for (size_t i = 0; i < size; i++)
    {
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
            A_Line.slope = (A_Line.y2 - A_Line.y1) / (A_Line.x2 - A_Line.x1);
            A_Line.y_intercept = A_Line.y1 - A_Line.slope * A_Line.x1;
            A_Line.center_x = A_Line.center_y = A_Line.direction = 0;
        }
        else
        {
            A_Line.slope = A_Line.y_intercept = A_Line.theta = 0;

            A_Line.center_x = Assembly.at(i).center_x;
            A_Line.center_y = Assembly.at(i).center_y;
            A_Line.direction = Assembly.at(i).direction;

            A_Line.theta_1 = atan2(A_Line.y1 - A_Line.center_y, A_Line.x1 - A_Line.center_x);
            A_Line.theta_2 = atan2(A_Line.y2 - A_Line.center_y, A_Line.x2 - A_Line.center_x);
        }
        if (A_Line.is_line)
        {
            A_Line.x_min = min(A_Line.x1, A_Line.x2);
            A_Line.x_max = max(A_Line.x1, A_Line.x2);
            A_Line.y_min = min(A_Line.y1, A_Line.y2);
            A_Line.y_max = max(A_Line.y1, A_Line.y2);
        }
        else
        {
            A_Line = Arc_Boundary_Meas_for_Assembly(A_Line);
        }

        Silkscreen.push_back(A_Line);
    }
    return Silkscreen;
}

Copper Arc_Boundary_Meas(Segment Arc) //moved to GRAPH
{
    Copper A_Arc;
    float first, second;
    float radius = hypot(Arc.x1 - Arc.center_x, Arc.y1 - Arc.center_y);
    first = Arc.theta_1;
    second = Arc.theta_2;
    if (first == second)
    {
        A_Arc.x_min = Arc.center_x - radius;
        A_Arc.x_max = Arc.center_x + radius;
        A_Arc.y_min = Arc.center_y - radius;
        A_Arc.y_max = Arc.center_y + radius;
        return A_Arc;
    }

    if (Arc.direction)
    {
        swap(first, second);
    }
    if (first > second)
    {
        A_Arc.x_max = (first >= 0 && second <= 0) ? Arc.center_x + radius : max(max(Arc.x1, Arc.x2), Arc.center_x);
        A_Arc.x_min = min(min(Arc.x1, Arc.x2), Arc.center_x);
        A_Arc.y_max = (first >= PI / 2 && second <= PI / 2) ? Arc.center_y + radius : max(max(Arc.y1, Arc.y2), Arc.center_y);
        A_Arc.y_min = (first >= -PI / 2 && second <= -PI / 2) ? Arc.center_y - radius : min(min(Arc.y1, Arc.y2), Arc.center_y);
    }
    else if (second < 0) // first < second < 0
    {
        A_Arc.x_max = Arc.center_x + radius;
        A_Arc.x_min = Arc.center_x - radius;
        A_Arc.y_max = Arc.center_y + radius;
        A_Arc.y_min = (first >= -PI / 2 || second <= -PI / 2) ? Arc.center_y - radius : min(min(Arc.y1, Arc.y2), Arc.center_y);
    }
    else if (first >= 0) // 0 <= first < second
    {
        A_Arc.x_max = Arc.center_x + radius;
        A_Arc.x_min = Arc.center_x - radius;
        A_Arc.y_max = (first >= PI / 2 || second <= PI / 2) ? Arc.center_y + radius : max(max(Arc.y1, Arc.y2), Arc.center_y);
        A_Arc.y_min = Arc.center_y - radius;
    }
    else // first < 0 < second
    {
        A_Arc.x_max = max(max(Arc.x1, Arc.x2), Arc.center_x);
        A_Arc.x_min = Arc.center_x - radius;
        A_Arc.y_max = (second <= PI / 2) ? Arc.center_y + radius : max(max(Arc.y1, Arc.y2), Arc.center_y);
        A_Arc.y_min = (first >= -PI / 2) ? Arc.center_y - radius : min(min(Arc.y1, Arc.y2), Arc.center_y);
    }

    /*A_Arc.x_max = (first >= 0 && second <= 0) ? Arc.center_x + radius : max(max(Arc.x1, Arc.x2), Arc.center_x);
    A_Arc.x_min = (first >= PI && second <= PI) ? Arc.center_x - radius : min(min(Arc.x1, Arc.x2), Arc.center_x);
    A_Arc.y_max = (first >= PI / 2 && second <= PI / 2) ? Arc.center_y + radius : max(max(Arc.y1, Arc.y2), Arc.center_y);
    A_Arc.y_min = (first >= -PI / 2 && second <= -PI / 2) ? Arc.center_y - radius : min(min(Arc.y1, Arc.y2), Arc.center_y);*/

    return A_Arc;
}

Segment Arc_Boundary_Meas_for_Assembly(Segment Arc) // duplicate function of Arc_Boundary_Meas !!!!
{
    Segment A_Arc;
    A_Arc = Arc;
    float first_angle, second_angle;
    float radius = hypot(Arc.x1 - Arc.center_x, Arc.y1 - Arc.center_y);
    first_angle = Arc.theta_1;
    second_angle = Arc.theta_2;
    if (first_angle == second_angle)
    {
        A_Arc.x_min = Arc.center_x - radius;
        A_Arc.x_max = Arc.center_x + radius;
        A_Arc.y_min = Arc.center_y - radius;
        A_Arc.y_max = Arc.center_y + radius;
        return A_Arc;
    }

    if (Arc.direction)
    {
        swap(first_angle, second_angle);
    }
    if (first_angle > second_angle)
    {
        A_Arc.x_max = (first_angle >= 0 && second_angle <= 0) ? Arc.center_x + radius : max(max(Arc.x1, Arc.x2), Arc.center_x);
        A_Arc.x_min = min(min(Arc.x1, Arc.x2), Arc.center_x);
        A_Arc.y_max = (first_angle >= PI / 2 && second_angle <= PI / 2) ? Arc.center_y + radius : max(max(Arc.y1, Arc.y2), Arc.center_y);
        A_Arc.y_min = (first_angle >= -PI / 2 && second_angle <= -PI / 2) ? Arc.center_y - radius : min(min(Arc.y1, Arc.y2), Arc.center_y);
    }
    else if (second_angle < 0) // first < second < 0
    {
        A_Arc.x_max = Arc.center_x + radius;
        A_Arc.x_min = Arc.center_x - radius;
        A_Arc.y_max = Arc.center_y + radius;
        A_Arc.y_min = (first_angle >= -PI / 2 || second_angle <= -PI / 2) ? Arc.center_y - radius : min(min(Arc.y1, Arc.y2), Arc.center_y);
    }
    else if (first_angle >= 0) // 0 <= first < second
    {
        A_Arc.x_max = Arc.center_x + radius;
        A_Arc.x_min = Arc.center_x - radius;
        A_Arc.y_max = (first_angle >= PI / 2 || second_angle <= PI / 2) ? Arc.center_y + radius : max(max(Arc.y1, Arc.y2), Arc.center_y);
        A_Arc.y_min = Arc.center_y - radius;
    }
    else // first < 0 < second
    {
        A_Arc.x_max = max(max(Arc.x1, Arc.x2), Arc.center_x);
        A_Arc.x_min = Arc.center_x - radius;
        A_Arc.y_max = (second_angle <= PI / 2) ? Arc.center_y + radius : max(max(Arc.y1, Arc.y2), Arc.center_y);
        A_Arc.y_min = (first_angle >= -PI / 2) ? Arc.center_y - radius : min(min(Arc.y1, Arc.y2), Arc.center_y);
    }

    /*A_Arc.x_max = (first >= 0 && second <= 0) ? Arc.center_x + radius : max(max(Arc.x1, Arc.x2), Arc.center_x);
    A_Arc.x_min = (first >= PI && second <= PI) ? Arc.center_x - radius : min(min(Arc.x1, Arc.x2), Arc.center_x);
    A_Arc.y_max = (first >= PI / 2 && second <= PI / 2) ? Arc.center_y + radius : max(max(Arc.y1, Arc.y2), Arc.center_y);
    A_Arc.y_min = (first >= -PI / 2 && second <= -PI / 2) ? Arc.center_y - radius : min(min(Arc.y1, Arc.y2), Arc.center_y);*/

    return A_Arc;
}

bool Input_Output::point_in_polygon(Point t, vector<Point> Assembly_Point, vector<vector<Point>> Arc_Points) // 運用射線法判斷點在圖形內外, move to point.cpp
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