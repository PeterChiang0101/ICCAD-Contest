#include <bits/stdc++.h>
#include "Point.h"
#include "Segment.h"
using namespace std;

vector<Point> Point_Extension(const vector<Segment> Assembly, const bool is_assembly) // 圖形外擴
{
    const int size = Assembly.size();
    vector<Point> Assembly_Points;
    vector<Point> Extended_Points;
    vector<vector<Point>> Arc_Dots;
    vector<Segment> Silkscreen;

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
        float Bisector_Slope = tan(Angle_Divided);               //角平分線
        double Point_Extend_Range;                               //點外擴距離
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
    }
    return Extended_Points;
}