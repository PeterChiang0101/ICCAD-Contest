#include <bits/stdc++.h>
#include "Point.h"
#include "Parameter.h"

using namespace std;


double POINT::dis2(const Point A, const Point B) //點A、B距離的平方
{
    return (double)(A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y);
}

double POINT::dist(const Point A, const Point B) //點A、B距離,with hypot
{
    return (double)hypot((A.x - B.x),(A.y - B.y));
}

double POINT::interpolate_x(const float y, const Point p1, const Point p2) // 待測點與圖形邊界交會的x值
{
    if (p1.y == p2.y)
        return (double)p1.x;
    return (double)(p1.x + (p2.x - p1.x) * (y - p1.y) / (p2.y - p1.y));
}

bool POINT::point_in_polygon(const Point t, const vector<Point> Assembly_Point, const vector<vector<Point>> Arc_Points) // 運用射線法判斷點在圖形內外
{
    size_t Assembly_size = Assembly_Point.size();
    size_t Arc_count = 0;
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

bool POINT::Point_Inside_Arc(const float Point_Theta,float Arc_Theta1, float Arc_Theta2, const bool is_CounterClockwise) // 確認點是否在圓弧內，已確認在該圓弧所屬的園內 (counterclockwise)
{
    if (!is_CounterClockwise)
    {
        swap(Arc_Theta1, Arc_Theta2);
    }
    if (Arc_Theta1 == Arc_Theta2) // 完整的圓，一定是在圓弧內
        return true;
    if (Arc_Theta1 > Arc_Theta2)
    {
        return((Point_Theta >= Arc_Theta1 || Point_Theta <= Arc_Theta2)?true:false);
    }
    else
    {
        return((Point_Theta >= Arc_Theta1 && Point_Theta <= Arc_Theta2)?true:false);
    }
}

bool POINT::In_Between_Lines(const Point test, const Point first, const Point last) // safe 
{    
    float min_x = min(first.x, last.x);
    float max_x = max(first.x, last.x);
    float min_y = min(first.y, last.y);
    float max_y = max(first.y, last.y);
    if (test.x >= min_x - Subtraction_Tolerance && test.x <= max_x + Subtraction_Tolerance && test.y >= min_y - Subtraction_Tolerance && test.y <= max_y + Subtraction_Tolerance)
        return true;
    else
        return false;
}

