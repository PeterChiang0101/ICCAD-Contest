// Silkscreen.cpp
// Implementation of Silkscreen Buffer and Modify the Silkscreen
#include <bits/stdc++.h>
#include "Silkscreen.h"
#include "Point.h"
#include "Segment.h"
#include "Graph.h"
#include "Parameter.h"

using namespace std;
// Silkscreen_Assembly not founded.

// constructor
Silkscreen::Silkscreen(float Silkscreenlen) :silkscreenlen{Silkscreenlen}
{
}
//combine the flow of function call into one function call.
vector<Graph> Silkscreen::Silkscreen_Assembly(const Graph assembly, const Graph silkscreen, const vector<Graph> copper)
{
    Graph copper_cut_silkscreen;
    vector<Graph> cont_silkscreen, Boarder_Condition;

    //output the untuned silkscreen(cut by copper)
    copper_cut_silkscreen = Untuned_Silkscreen(silkscreen, copper);
    //delete too short silkscreen
    cont_silkscreen = Delete_Short_Silkscreen(copper_cut_silkscreen);
    //additional silkscreen to cover the whole assembly edge
    Boarder_Condition = fit_boarder_condition(cont_silkscreen, silkscreen, assembly, copper);
    
    return Boarder_Condition;
}
//-----------------------------------Untuned Silkscreen-----------------------------------
// 未切割的絲印 與 外擴的銅箔
Graph Silkscreen::Untuned_Silkscreen(Graph Silkscreen_Original, vector<Graph> Coppers)
{
    Graph Silkscreen_Cut_Complete; // 切割完成的完整絲印
    Graph Silkscreen_Cut_Part;     // 切割完成的一條絲印
    size_t Silkscreen_Org_Size = Silkscreen_Original.segment.size();

    for (size_t i = 0; i < Silkscreen_Org_Size; i++)
    {
        Silkscreen_Cut_Part.segment.clear();
        Silkscreen_Cut_Part = Cut_Silkscreen_by_Copper(Silkscreen_Original.segment.at(i), Coppers);
        size_t Silkscreen_Cut_Part_Size = Silkscreen_Cut_Part.segment.size();
        for (size_t j = 0; j < Silkscreen_Cut_Part_Size; j++)
        {
            if (Silkscreen_Cut_Part.segment.at(j).is_line)
            { // find the Min/Max of the single segment
                Silkscreen_Cut_Part.segment.at(j).detail.x_min = min(Silkscreen_Cut_Part.segment.at(j).x1, Silkscreen_Cut_Part.segment.at(j).x2);
                Silkscreen_Cut_Part.segment.at(j).detail.x_max = max(Silkscreen_Cut_Part.segment.at(j).x1, Silkscreen_Cut_Part.segment.at(j).x2);
                Silkscreen_Cut_Part.segment.at(j).detail.y_min = min(Silkscreen_Cut_Part.segment.at(j).y1, Silkscreen_Cut_Part.segment.at(j).y2);
                Silkscreen_Cut_Part.segment.at(j).detail.y_max = max(Silkscreen_Cut_Part.segment.at(j).y1, Silkscreen_Cut_Part.segment.at(j).y2);
            }
            else
            {
                Silkscreen_Cut_Part.segment.at(j) = Arc_Boundary_Meas_for_Assembly(Silkscreen_Cut_Part.segment.at(j));
            }
        }
        // TODO: find the Min/Max of the whole silkscreen.
        Silkscreen_Cut_Complete.segment.insert(Silkscreen_Cut_Complete.segment.end(), Silkscreen_Cut_Part.segment.begin(), Silkscreen_Cut_Part.segment.end());
    }
    return Silkscreen_Cut_Complete;
}
// not complete yet
Graph Silkscreen::Cut_Silkscreen_by_Copper(Segment Silkscreen_Piece, vector<Graph> Coppers)
{
    size_t Copper_size = Coppers.size();
    Graph Single_Silkscreen_Cut_Complete;
    Graph total_copper_cut_segments; // 取所有須切割區域的聯集
    Graph copper_cut_segments;       // 一個copper所遮住這條絲印的部分
    Segment A_Line;
    Copper_cut_segments.resize(Copper_size);

    // 僅處理直線極值，需增加圓弧極值

    // Single_Silkscreen_Cut_Complete.push_back(Silkscreen_Piece);
    for (size_t i = 0; i < Copper_size; i++) // 每次處理一個copper
    {
        if (Silkscreen_Piece.detail.x_min > Coppers.at(i).x_max || Silkscreen_Piece.detail.x_max < Coppers.at(i).x_min || Silkscreen_Piece.detail.y_min > Coppers.at(i).y_max || Silkscreen_Piece.detail.y_max < Coppers.at(i).y_min) // 如果這條絲印不在這個copper的區域內
            continue;
        copper_cut_segments = silkscreen_cut_single_copper(Silkscreen_Piece, Coppers.at(i)); // 絲印與單一copper的交集線段
        copper_cut_segments.x_max = (float)i;//record the copper id into x_max 
        total_copper_cut_segments.segment.insert(total_copper_cut_segments.segment.end(), copper_cut_segments.segment.begin(), copper_cut_segments.segment.end()); // 線段之間可能有交集
        Copper_cut_segments.at(i).segment.insert(Copper_cut_segments.at(i).segment.end(),copper_cut_segments.segment.begin(),copper_cut_segments.segment.end());//
        // use the Xmax the record the copper id.
    }
    total_copper_cut_segments = Segment_Sort(Silkscreen_Piece, total_copper_cut_segments); // 將線段排序

    size_t total_segment = total_copper_cut_segments.segment.size(); // 聯集完

    if (total_segment == 2) // 只有頭跟尾，不需要切割
    {
        Single_Silkscreen_Cut_Complete.segment.push_back(Silkscreen_Piece);
        return Single_Silkscreen_Cut_Complete;
    }
    if (Silkscreen_Piece.is_line)
        A_Line.is_line = true;
    else
    {
        A_Line.is_line = false;
        A_Line.center_x = Silkscreen_Piece.center_x;
        A_Line.center_y = Silkscreen_Piece.center_y;
        A_Line.is_CCW = Silkscreen_Piece.is_CCW;
    }

    for (int i = 1; i < total_segment; i++)
    {
        if (total_copper_cut_segments.segment.at(i).x1 == total_copper_cut_segments.segment.at(i - 1).x2 && total_copper_cut_segments.segment.at(i).y1 == total_copper_cut_segments.segment.at(i - 1).y2) // 共點
            continue;

        A_Line.x1 = total_copper_cut_segments.segment.at(i - 1).x2;
        A_Line.y1 = total_copper_cut_segments.segment.at(i - 1).y2;
        A_Line.x2 = total_copper_cut_segments.segment.at(i).x1;
        A_Line.y2 = total_copper_cut_segments.segment.at(i).y1;
        A_Line.detail.x_max = total_copper_cut_segments.segment.at(i-1).detail.x_max;//the segment of the copper
        A_Line.detail.x_min = total_copper_cut_segments.x_max;// the ID of the copper
        A_Line.detail.y_max = total_copper_cut_segments.segment.at(i-1).detail.x_max;//the segment of the copper
        A_Line.detail.y_min = total_copper_cut_segments.x_max;// the ID of the copper
        Single_Silkscreen_Cut_Complete.segment.push_back(A_Line); // 最終切完的結果
    }
    return Single_Silkscreen_Cut_Complete; // 回傳切割完的結果
}
// not complete yet
Graph Silkscreen::silkscreen_cut_single_copper(Segment Silkscreen_Piece, Graph Single_Copper) // 切割與單一銅箔交會的絲印
{
    size_t Copper_Line_size = Single_Copper.segment.size();
    vector<vector<Point>> Arc_Dots;
    vector<Point> Copper_Points;
    vector<Point_ID> Intersection_Points;
    vector<Intersection> Intersection_Info;
    Point_ID first_point, last_point;
    Point_ID Point_Intersect;
    bool first_inside, last_inside;
    //record the start and end points of silkscreen
    first_point.point.x = Silkscreen_Piece.x1;
    first_point.point.y = Silkscreen_Piece.y1;
    last_point.point.x = Silkscreen_Piece.x2;
    last_point.point.y = Silkscreen_Piece.y2;
    //Copper outline points
    Copper_Points = Line_to_Point(Single_Copper);
    Arc_Dots = Arc_Optimization(Single_Copper);

    first_inside = point_in_polygon(first_point.point, Copper_Points, Arc_Dots);
    last_inside = point_in_polygon(last_point.point, Copper_Points, Arc_Dots);

    Intersection_Points.push_back(first_point);
    for (size_t i = 0; i < Copper_Line_size; i++)
    {
        Point first_copper_point, second_copper_point;
        vector<Point> Intersection_Points_temp;
        first_copper_point.x = Single_Copper.segment.at(i).x1;
        first_copper_point.y = Single_Copper.segment.at(i).y1;
        second_copper_point.x = Single_Copper.segment.at(i).x2;
        second_copper_point.y = Single_Copper.segment.at(i).y2;
        if (Silkscreen_Piece.is_line && Single_Copper.segment.at(i).is_line) // 如果絲印是線段，銅箔也是線段
        {
            Point_Intersect.point = intersection(first_point.point, last_point.point, first_copper_point, second_copper_point); // 線與線交會點
            if (Point_Intersect.point.x != INFINITY && Point_Intersect.point.y != INFINITY)
                Point_Intersect.ID = (int)i;
                Intersection_Points.push_back(Point_Intersect);
        }
        else if (Silkscreen_Piece.is_line && !Single_Copper.segment.at(i).is_line) // 如果絲印是線段，銅箔是圓弧
        {
            // 計算線與圓弧的交會點
            Intersection_Points_temp = intersection_between_line_and_arc(Single_Copper.segment.at(i), first_point.point, last_point.point);
            for(size_t j = 0; j < Intersection_Points_temp.size(); j++)
            {
                Point_Intersect.ID = i;
                Point_Intersect.point = Intersection_Points_temp.at(j);
                Intersection_Points.push_back(Point_Intersect);
            }
            //Intersection_Points.insert(Intersection_Points.end(), Intersection_Points_temp.begin(), Intersection_Points_temp.end());
        }
        else if (!Silkscreen_Piece.is_line && Single_Copper.segment.at(i).is_line) // 如果絲印是圓弧，銅箔是線段
        {
            // 計算線與圓弧的交會點
            Intersection_Points_temp = intersection_between_line_and_arc(Silkscreen_Piece, first_copper_point, second_copper_point);
            for(size_t j = 0; j < Intersection_Points_temp.size(); j++)
            {
                Point_Intersect.ID = i;
                Point_Intersect.point = Intersection_Points_temp.at(j);
                Intersection_Points.push_back(Point_Intersect);
            }
            //Intersection_Points.insert(Intersection_Points.end(), Intersection_Points_temp.begin(), Intersection_Points_temp.end());
        }
        else if (!Silkscreen_Piece.is_line && !Single_Copper.segment.at(i).is_line) // 如果絲印是圓弧，銅箔也是圓弧
        {
            Intersection_Points_temp = intersection_between_arc_and_arc(Silkscreen_Piece, Single_Copper.segment.at(i));
            for(size_t j = 0; j < Intersection_Points_temp.size(); j++)
            {
                Point_Intersect.ID = i;
                Point_Intersect.point = Intersection_Points_temp.at(j);
                Intersection_Points.push_back(Point_Intersect);
            }
            //Intersection_Points.insert(Intersection_Points.end(), Intersection_Points_temp.begin(), Intersection_Points_temp.end());
        }
    }
    Intersection_Points.push_back(last_point);

    Intersection_Points = Point_Sort(Silkscreen_Piece, Intersection_Points);

    Graph Cut_Lines;
    Segment A_Line;
    size_t Intersection_Points_size = Intersection_Points.size();

    for (size_t i = 0; i < Intersection_Points_size; i++) // 量出需要被切割的線段
    {
        if ((i == 0 && !first_inside) || (i == Intersection_Points_size - 1 && !last_inside))
            continue; // 兩端點在外面可以直接略過
        A_Line.x1 = Intersection_Points.at(i).point.x;
        A_Line.y1 = Intersection_Points.at(i).point.y;
        i++;
        A_Line.x2 = Intersection_Points.at(i).point.x;
        A_Line.y2 = Intersection_Points.at(i).point.y;
        //use the X_max to record the intersection line of copper(Warning!!)
        A_Line.detail.x_max = (float)Intersection_Points.at(i).ID;
        // for Arc
        if (!Silkscreen_Piece.is_line)
        {
            A_Line.is_line = false;
            A_Line.center_x = Silkscreen_Piece.center_x;
            A_Line.center_y = Silkscreen_Piece.center_y;
            A_Line.detail.theta_1 = atan2(A_Line.y1 - A_Line.center_y, A_Line.x1 - A_Line.center_x);
            A_Line.detail.theta_2 = -5; // simplied,set to unreachable number.
            A_Line.is_CCW = Silkscreen_Piece.is_CCW;
        }
        else
        {
            A_Line.is_line = true;
        }
        Cut_Lines.segment.push_back(A_Line);
    }

    return Cut_Lines;
}

Segment Silkscreen::Arc_Boundary_Meas_for_Assembly(const Segment Arc)
{
    Segment A_Arc;
    A_Arc = Arc;
    float first_angle, second_angle;
    float radius = hypot(Arc.x1 - Arc.center_x, Arc.y1 - Arc.center_y);
    first_angle = Arc.detail.theta_1;
    second_angle = Arc.detail.theta_2;
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

    /*A_Arc.x_max = (first >= 0 && second <= 0) ? Arc.center_x + radius : max(max(Arc.x1, Arc.x2), Arc.center_x);
    A_Arc.x_min = (first >= PI && second <= PI) ? Arc.center_x - radius : min(min(Arc.x1, Arc.x2), Arc.center_x);
    A_Arc.y_max = (first >= PI / 2 && second <= PI / 2) ? Arc.center_y + radius : max(max(Arc.y1, Arc.y2), Arc.center_y);
    A_Arc.y_min = (first >= -PI / 2 && second <= -PI / 2) ? Arc.center_y - radius : min(min(Arc.y1, Arc.y2), Arc.center_y);*/

    return A_Arc;
}

vector<Point> Silkscreen::intersection_between_arc_and_arc(const Segment Arc1, const Segment Arc2)
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
            if (Point_Inside_Arc(P1_Arc1_Theta, Arc1.detail.theta_1, Arc1.detail.theta_2, Arc1.is_CCW) && Point_Inside_Arc(P1_Arc2_Theta, Arc2.detail.theta_1, Arc2.detail.theta_2, Arc2.is_CCW))
                Intersection_Points.push_back(P1);
            if (Point_Inside_Arc(P2_Arc1_Theta, Arc1.detail.theta_1, Arc1.detail.theta_2, Arc1.is_CCW) && Point_Inside_Arc(P2_Arc2_Theta, Arc2.detail.theta_1, Arc2.detail.theta_2, Arc2.is_CCW))
                Intersection_Points.push_back(P2);
            // 需判斷點是否在圓弧，用 Point_Inside_Arc 函式
        }
        return Intersection_Points;
    }
    return vector<Point>();
}
// Sorting function:Segment and Point
Graph Silkscreen::Segment_Sort(Segment Silkscreen_Piece, Graph total_copper_cut_segments)
{
    // 由 x1 or y1 小至大排序
    // 開頭Segment 為 Silkscreen_Piece.x1, Silkscreen_Piece.y1, Silkscreen_Piece.x1, Silkscreen_Piece.y1
    // 排序
    // 結尾Segment 為 Silkscreen_Piece.x2, Silkscreen_Piece.y2, Silkscreen_Piece.x2, Silkscreen_Piece.y2
    Graph Cut_Silkscreen;
    Segment Start_point, End_point;
    bool Sort_as_Line = false; // Determine whether can use line sorting case or not.
    bool Seperate_x_dir = true;

    Start_point.x1 = Start_point.x2 = Silkscreen_Piece.x1;
    Start_point.y1 = Start_point.y2 = Silkscreen_Piece.y1;

    End_point.x1 = End_point.x2 = Silkscreen_Piece.x2;
    End_point.y1 = End_point.y2 = Silkscreen_Piece.y2;
    Cut_Silkscreen.segment.push_back(Start_point);
    // Determine whether can be sorted as line segments.
    if (Silkscreen_Piece.is_line)
    {
        Sort_as_Line = true;
    }
    else
    {
        if ((Silkscreen_Piece.detail.theta_1 >= 0 && Silkscreen_Piece.detail.theta_2 >= 0) || (Silkscreen_Piece.detail.theta_1 <= 0 && Silkscreen_Piece.detail.theta_2 <= 0))
        { // Segment Arc is either in upper semicircle or upper semicircle -> determine by X1 and X2.
            Sort_as_Line = true;
        }
    }

    if (Sort_as_Line)
    {
        if (Silkscreen_Piece.x1 - Silkscreen_Piece.x2 < -Subtraction_Tolerance)
        {
            sort(total_copper_cut_segments.segment.begin(), total_copper_cut_segments.segment.end(), sort_increase_Segment);
        }
        else if (Silkscreen_Piece.x1 - Silkscreen_Piece.x2 > Subtraction_Tolerance)
        {
            sort(total_copper_cut_segments.segment.begin(), total_copper_cut_segments.segment.end(), sort_decrease_Segment);
        }
        else if (Silkscreen_Piece.y1 - Silkscreen_Piece.y2 < -Subtraction_Tolerance)
        {
            sort(total_copper_cut_segments.segment.begin(), total_copper_cut_segments.segment.end(), sort_increase_Segment);
        }
        else if (Silkscreen_Piece.y1 - Silkscreen_Piece.y2 > Subtraction_Tolerance)
        {
            sort(total_copper_cut_segments.segment.begin(), total_copper_cut_segments.segment.end(), sort_decrease_Segment);
        }
        else
        { // ERROR STATUS: X1=X2, Y1=Y2;Action: sort_increase_Segment
            sort(total_copper_cut_segments.segment.begin(), total_copper_cut_segments.segment.end(), sort_increase_Segment);
            cerr << "ERROR STATUS:Silkscreen_Piece points are the same." << endl;
        }
    }
    else
    {
        size_t across_neg_x = 0; // find the cloest point of X-axis
        // Sort Segment by direction, regradless the Segment across the x-axis
        if (Silkscreen_Piece.is_CCW) // CCW
        {
            sort(total_copper_cut_segments.segment.begin(), total_copper_cut_segments.segment.end(), sort_increase_Arc);
        }
        else // CW
        {
            sort(total_copper_cut_segments.segment.begin(), total_copper_cut_segments.segment.end(), sort_decrease_Arc);
        }

        // Use theta and direction to determine across the negative x-asix
        if ((Silkscreen_Piece.detail.theta_1 > 0) && (Silkscreen_Piece.detail.theta_2 < 0))
        {
            if (Silkscreen_Piece.is_CCW)
            {
                Seperate_x_dir = true; // Seperate by PI (rad)
            }
            else
            {
                Seperate_x_dir = false; // Seperate by 0 (rad)
            }
        }
        else if ((Silkscreen_Piece.detail.theta_2 > 0) && (Silkscreen_Piece.detail.theta_1 < 0))
        {
            if (Silkscreen_Piece.is_CCW)
            {
                Seperate_x_dir = false; // Seperate by 0 (rad)
            }
            else
            {
                Seperate_x_dir = true; // Seperate by PI (rad)
            }
        }
        else
        { // ERROR STATUS
            cerr << "ERROR STATUS:Initialize may required to identify the \'theta\'." << endl;
        }

        if (Seperate_x_dir)
        { // find the first segment across the negative x direction
            for (size_t i = 0; i < total_copper_cut_segments.segment.size(); i++)
            {

                if (Silkscreen_Piece.is_CCW && total_copper_cut_segments.segment.at(i).detail.theta_1 > 0)
                {
                    across_neg_x = i;
                    break;
                }
                else if (!Silkscreen_Piece.is_CCW && total_copper_cut_segments.segment.at(i).detail.theta_1 < 0)
                {
                    across_neg_x = i;
                    break;
                }
            }
            if (Seperate_x_dir > 0)
            { // rearrange the segments for those that across the negative x-asix
                total_copper_cut_segments.segment.insert(total_copper_cut_segments.segment.end(), total_copper_cut_segments.segment.begin(), total_copper_cut_segments.segment.begin() + across_neg_x);
                total_copper_cut_segments.segment.erase(total_copper_cut_segments.segment.begin(), total_copper_cut_segments.segment.begin() + across_neg_x);
            }
        }
    }
    Cut_Silkscreen.segment.insert(Cut_Silkscreen.segment.end(), total_copper_cut_segments.segment.begin(), total_copper_cut_segments.segment.end());
    Cut_Silkscreen.segment.push_back(End_point);

    return Cut_Silkscreen;
}

bool Silkscreen::sort_increase_Segment(const Segment L1, const Segment L2)
{

    if (abs(L1.x1 - L2.x1) < Subtraction_Tolerance)
    {
        return (L1.y1 < L2.y1);
    }
    else
    {
        return (L1.x1 < L2.x1);
    }
}
bool Silkscreen::sort_decrease_Segment(const Segment L1, const Segment L2)
{

    if (abs(L1.x1 - L2.x1) < Subtraction_Tolerance)
    {
        return (L1.y1 > L2.y1);
    }
    else
    {
        return (L1.x1 > L2.x1);
    }
}

bool Silkscreen::sort_decrease_Arc(const Segment L1, const Segment L2)
{
    return (L1.detail.theta_1 < L2.detail.theta_1);
}

bool Silkscreen::sort_increase_Arc(const Segment L1, const Segment L2)
{
    return (L1.detail.theta_1 > L2.detail.theta_1);
}

vector<Point_ID> Silkscreen::Point_Sort(const Segment Silkscreen_Piece, vector<Point_ID> Intersection_Points)
{
    // Warning!! this version will modify the input array and return it back.
    size_t final_point = Intersection_Points.size() - 1;
    bool Sort_as_Line = false;

    if (Silkscreen_Piece.is_line)
    {
        Sort_as_Line = true;
    }
    else
    {
        if ((Silkscreen_Piece.detail.theta_1 >= 0 && Silkscreen_Piece.detail.theta_2 >= 0) || (Silkscreen_Piece.detail.theta_1 <= 0 && Silkscreen_Piece.detail.theta_2 <= 0))
        {
            Sort_as_Line = true;
        }
    }

    if (Sort_as_Line)
    {
        if ((Intersection_Points.at(0).point.x) - (Intersection_Points.at(final_point).point.x) > Subtraction_Tolerance)
        {
            sort(Intersection_Points.begin(), Intersection_Points.end(), sort_decrease_points);
        }
        else if ((Intersection_Points.at(0).point.x) - (Intersection_Points.at(final_point).point.x) < -Subtraction_Tolerance)
        {
            sort(Intersection_Points.begin(), Intersection_Points.end(), sort_increase_points);
        }
        else if ((Intersection_Points.at(0).point.y) - (Intersection_Points.at(final_point).point.y) < -Subtraction_Tolerance)
        {
            sort(Intersection_Points.begin(), Intersection_Points.end(), sort_increase_points);
        }
        else
        {
            sort(Intersection_Points.begin(), Intersection_Points.end(), sort_decrease_points);
        }
    }
    else
    {
        // find the X-axis
        double X_axis = Silkscreen_Piece.center_y;
        vector<Point_ID> upper_points, lower_points;
        // Seperate points into upper and lower semicircle
        for (size_t i = 0; i < Intersection_Points.size(); i++)
        {
            if (Intersection_Points.at(i).point.y > X_axis)
            {
                upper_points.push_back(Intersection_Points.at(i));
            }
            else
            {
                lower_points.push_back(Intersection_Points.at(i));
            }
        }

        if (Silkscreen_Piece.is_CCW) // CCW
        {
            sort(upper_points.begin(), upper_points.end(), sort_decrease_points); // upper
            sort(lower_points.begin(), lower_points.end(), sort_increase_points); // lower
        }
        else // CW
        {
            sort(upper_points.begin(), upper_points.end(), sort_increase_points); // upper
            sort(lower_points.begin(), lower_points.end(), sort_decrease_points); // lower
        }

        Intersection_Points.clear();
        if (Silkscreen_Piece.detail.theta_1 > 0 && Silkscreen_Piece.detail.theta_2 < 0)
        {
            Intersection_Points = upper_points;
            Intersection_Points.insert(Intersection_Points.end(), lower_points.begin(), lower_points.end());
        }
        else
        {
            Intersection_Points = lower_points;
            Intersection_Points.insert(Intersection_Points.end(), upper_points.begin(), upper_points.end());
        }
    }
    return Intersection_Points;
}

bool Silkscreen::sort_decrease_points(const Point_ID p1, const Point_ID p2)
{
    if (abs(p1.point.x - p2.point.x) > Subtraction_Tolerance)
    {
        return (p1.point.x > p2.point.x);
    }
    else
    {
        return (p1.point.y > p2.point.y);
    }
}
bool Silkscreen::sort_increase_points(const Point_ID p1, const Point_ID p2)
{
    if (abs(p1.point.x - p2.point.x) > Subtraction_Tolerance)
    {
        return (p1.point.x < p2.point.x);
    }
    else
    {
        return (p1.point.y < p2.point.y);
    }
}
//-----------------------------------End Untuned_Silkscreen -----------------------------------
//-----------------------------------Delete short Silkscreen and its dependencies-----------------------------------

vector<Graph> Silkscreen::Delete_Short_Silkscreen(Graph Silkscreen) //刪除過短的絲印,可能會刪到Assembly極值絲印
{
    float len;
    vector<Graph> All_Continuous;
    vector<Intersection> points;
    All_Continuous = Find_Continuous_Segment(Silkscreen);
    size_t Continue_size = All_Continuous.size();
    for (size_t i = 0; i < Continue_size; i++)
    {
        size_t A_Continuous_Segment_size = All_Continuous.at(i).segment.size();
        len = 0;
        // the following for loop can be replaced with:
        // len = Calculate_Silkscreen_length(All_Continuous.at(i));
        for (size_t j = 0; j < A_Continuous_Segment_size; j++)
        {
            if (All_Continuous.at(i).segment.at(j).is_line)
            {
                len += hypot(All_Continuous.at(i).segment.at(j).x2 - All_Continuous.at(i).segment.at(j).x1, All_Continuous.at(i).segment.at(j).y2 - All_Continuous.at(i).segment.at(j).y1);
            }
            else // arc
            {
                float circumference = hypot(All_Continuous.at(i).segment.at(j).x2 - All_Continuous.at(i).segment.at(j).center_x, All_Continuous.at(i).segment.at(j).y2 - All_Continuous.at(i).segment.at(j).center_y);
                float angle_1, angle_2, angle_between;
                angle_1 = All_Continuous.at(i).segment.at(j).detail.theta_1;
                angle_2 = All_Continuous.at(i).segment.at(j).detail.theta_2;
                if (All_Continuous.at(i).segment.at(j).is_CCW)
                    swap(angle_1, angle_2);
                angle_between = angle_1 - angle_2;
                if (angle_between <= 0)
                    angle_between += 2 * PI;
                float partial_circumference = circumference * angle_between;

                len += partial_circumference;
            }
            if (len >= silkscreenlen)
                break;
        }
        if (len < silkscreenlen)
        {
            All_Continuous.erase(All_Continuous.begin() + i);
            i--;
            Continue_size--;
        }
    }
    //find the intersection of continuous silkscreen and coppers.
    Find_Intersection_Copper_Silkscreen(All_Continuous);
    return All_Continuous;
}

vector<Graph> Silkscreen::Find_Continuous_Segment(Graph Silkscreen)
{
    vector<Graph> continue_segment;
    Graph continue_temp;
    size_t Silkscreen_size = Silkscreen.segment.size();
    if (Silkscreen_size == 0)
    {
        cout << "Error: Find_Continuous_Segment: Silkscreen is empty" << endl;
        return continue_segment;
    }
    // set the continue_segment X_min, X_max, Y_min and Y_max to -5000(unused variables)
    continue_temp.x_min = continue_temp.x_max = (float)-5000;
    continue_temp.y_min = continue_temp.y_max = (float)-5000;

    for (size_t i = 0; i < Silkscreen_size; i++)
    {
        if (i == Silkscreen_size - 1)
        {
            continue_temp.segment.push_back(Silkscreen.segment.at(i));

            if ((Silkscreen.segment.at(i).x2 == Silkscreen.segment.at(0).x1) && (Silkscreen.segment.at(i).y2 == Silkscreen.segment.at(0).y1)) // 如果最後一條線是連接到第一條線的，則加入
            {
                continue_temp.segment.insert(continue_temp.segment.end(), continue_segment.at(0).segment.begin(), continue_segment.at(0).segment.end());
                continue_segment.erase(continue_segment.begin());
            }
            continue_segment.push_back(continue_temp);
        }
        else
        {
            continue_temp.segment.push_back(Silkscreen.segment.at(i));
            if ((Silkscreen.segment.at(i).x2 != Silkscreen.segment.at(i + 1).x1) || (Silkscreen.segment.at(i).y2 != Silkscreen.segment.at(i + 1).y1))
            {
                // continue_temp.segment.push_back(Silkscreen.segment.at(i));
                continue_segment.push_back(continue_temp);
                // clear the continue segment
                continue_temp.segment.clear();
            }
        }
    }
    return continue_segment;
}

void Silkscreen::Find_Intersection_Copper_Silkscreen(const vector<Graph> continuous_silkscreen)
{
    Intersection part;
    for(size_t i = 0; i < continuous_silkscreen.size(); i++)
    {
        size_t continue_silkscreen_graph_size = continuous_silkscreen.at(i).segment.size();
        //record the copper of the edge(intersected) silkscreen.
        if(continue_silkscreen_graph_size > 0)//find the continue_silkscreen head and tail
        {
            //head segment
            part.copper_segment = (int)continuous_silkscreen.at(i).segment.at(0).detail.x_max;
            part.copper_ID = (int)continuous_silkscreen.at(i).segment.at(0).detail.x_min;
            part.cont_silkscreen = i;
            part.intersection_point.x = continuous_silkscreen.at(i).segment.at(0).x1;
            part.intersection_point.y = continuous_silkscreen.at(i).segment.at(0).y1;
            intersect_points.push_back(part);
            //tail segment
            part.copper_segment = (int)continuous_silkscreen.at(i).segment.at(continue_silkscreen_graph_size-1).detail.y_max;
            part.copper_ID = (int)continuous_silkscreen.at(i).segment.at(continue_silkscreen_graph_size-1).detail.y_min;
            part.intersection_point.x = continuous_silkscreen.at(i).segment.at(continue_silkscreen_graph_size-1).x2;
            part.intersection_point.y = continuous_silkscreen.at(i).segment.at(continue_silkscreen_graph_size-1).y2;
            intersect_points.push_back(part);
        }
    }
}
// -----------------------------------END Delete short Silkscreen function-----------------------------------
// ----------------------------------- fit_boarder_condition and its dependencies -----------------------------------
vector<Graph> Silkscreen::fit_boarder_condition(vector<Graph> Silkscreen, Graph Uncut_Silkscreen, Graph Assembly, vector<Graph> Copper_Expanded)
{
    size_t Assembly_size = Assembly.segment.size();
    size_t Silkscreen_size = Silkscreen.size();
    int Silkscreen_piece_size;

    float Assembly_x_min = Assembly.segment.at(0).detail.x_min;
    float Assembly_x_max = Assembly.segment.at(0).detail.x_max;
    float Assembly_y_min = Assembly.segment.at(0).detail.y_min;
    float Assembly_y_max = Assembly.segment.at(0).detail.y_max;

    int Uppest_Assembly_index = 0;   // assembly 最上面的線
    int Lowest_Assembly_index = 0;   // assembly 最下面的線
    int Leftest_Assembly_index = 0;  // assembly 最左邊的線
    int Rightest_Assembly_index = 0; // assembly 最右邊的線

    float Silkscreen_x_min = Silkscreen.at(0).segment.at(0).detail.x_min;
    float Silkscreen_x_max = Silkscreen.at(0).segment.at(0).detail.x_max;
    float Silkscreen_y_min = Silkscreen.at(0).segment.at(0).detail.y_min;
    float Silkscreen_y_max = Silkscreen.at(0).segment.at(0).detail.y_max;

    vector<vector<Segment>> Silkscreen_fit_Condition;

    for (int i = 0; i < Assembly_size; i++) // 找assembly極值
    {
        if (Assembly.segment.at(i).detail.x_min < Assembly_x_min)
        {
            Assembly_x_min = Assembly.segment.at(i).detail.x_min;
            Leftest_Assembly_index = i;
        }
        if (Assembly.segment.at(i).detail.x_max > Assembly_x_max)
        {
            Assembly_x_max = Assembly.segment.at(i).detail.x_max;
            Rightest_Assembly_index = i;
        }
        if (Assembly.segment.at(i).detail.y_min < Assembly_y_min)
        {
            Assembly_y_min = Assembly.segment.at(i).detail.y_min;
            Lowest_Assembly_index = i;
        }
        if (Assembly.segment.at(i).detail.y_max > Assembly_y_max)
        {
            Assembly_y_max = Assembly.segment.at(i).detail.y_max;
            Uppest_Assembly_index = i;
        }
    }
    for (int i = 0; i < Silkscreen_size; i++) // 找絲印極值
    {
        Silkscreen_piece_size = Silkscreen.at(i).segment.size();
        for (int j = 0; j < Silkscreen_piece_size; j++)
        {
            if (Silkscreen.at(i).segment.at(j).detail.x_min < Silkscreen_x_min)
            {
                Silkscreen_x_min = Silkscreen.at(i).segment.at(j).detail.x_min;
            }
            if (Silkscreen.at(i).segment.at(j).detail.x_max > Silkscreen_x_max)
            {
                Silkscreen_x_max = Silkscreen.at(i).segment.at(j).detail.x_max;
            }
            if (Silkscreen.at(i).segment.at(j).detail.y_min < Silkscreen_y_min)
            {
                Silkscreen_y_min = Silkscreen.at(i).segment.at(j).detail.y_min;
            }
            if (Silkscreen.at(i).segment.at(j).detail.y_max > Silkscreen_y_max)
            {
                Silkscreen_y_max = Silkscreen.at(i).segment.at(j).detail.y_max;
            }
        }
    }

    Point extremum;

    if (Silkscreen_x_min > Assembly_x_min) // 左方沒包住
    {
        extremum.x = Assembly.segment.at(Leftest_Assembly_index).detail.x_min;
        if (extremum.x == Assembly.segment.at(Leftest_Assembly_index).x1)
            extremum.y = Assembly.segment.at(Leftest_Assembly_index).y1;
        else if ((extremum.x == Assembly.segment.at(Leftest_Assembly_index).x2))
            extremum.y = Assembly.segment.at(Leftest_Assembly_index).y2;
        else
            extremum.y = Assembly.segment.at(Leftest_Assembly_index).center_y - hypot(Assembly.segment.at(Leftest_Assembly_index).x2 - Assembly.segment.at(Leftest_Assembly_index).center_x, Assembly.segment.at(Leftest_Assembly_index).y2 - Assembly.segment.at(Leftest_Assembly_index).center_x);
        Silkscreen = Add_Excess_Silkscreen_For_Boarder_Condition(Silkscreen, extremum, Copper_Expanded, 1, Assembly);
    }
    if (Silkscreen_x_max < Assembly_x_max) // 右方沒包住
    {
        extremum.x = Assembly.segment.at(Rightest_Assembly_index).detail.x_max;
        if (extremum.x == Assembly.segment.at(Rightest_Assembly_index).x1)
            extremum.y = Assembly.segment.at(Rightest_Assembly_index).y1;
        else if ((extremum.x == Assembly.segment.at(Rightest_Assembly_index).x2))
            extremum.y = Assembly.segment.at(Rightest_Assembly_index).y2;
        else
            extremum.y = Assembly.segment.at(Rightest_Assembly_index).center_y + hypot(Assembly.segment.at(Rightest_Assembly_index).x2 - Assembly.segment.at(Rightest_Assembly_index).center_x, Assembly.segment.at(Rightest_Assembly_index).y2 - Assembly.segment.at(Rightest_Assembly_index).center_x);
        Silkscreen = Add_Excess_Silkscreen_For_Boarder_Condition(Silkscreen, extremum, Copper_Expanded, 2, Assembly);
    }
    if (Silkscreen_y_min > Assembly_y_min) // 下方沒包住
    {
        extremum.y = Assembly.segment.at(Lowest_Assembly_index).detail.y_min;
        if (extremum.y == Assembly.segment.at(Lowest_Assembly_index).y1)
            extremum.x = Assembly.segment.at(Lowest_Assembly_index).x1;
        else if ((extremum.y == Assembly.segment.at(Lowest_Assembly_index).y2))
            extremum.x = Assembly.segment.at(Lowest_Assembly_index).x2;
        else
            extremum.x = Assembly.segment.at(Lowest_Assembly_index).center_x - hypot(Assembly.segment.at(Lowest_Assembly_index).y2 - Assembly.segment.at(Lowest_Assembly_index).center_y, Assembly.segment.at(Lowest_Assembly_index).x2 - Assembly.segment.at(Lowest_Assembly_index).center_y);
        Silkscreen = Add_Excess_Silkscreen_For_Boarder_Condition(Silkscreen, extremum, Copper_Expanded, 3, Assembly);
    }
    if (Silkscreen_y_max < Assembly_y_max) // 上方沒包住
    {
        extremum.y = Assembly.segment.at(Uppest_Assembly_index).detail.y_max;
        if (extremum.y == Assembly.segment.at(Uppest_Assembly_index).y1)
            extremum.x = Assembly.segment.at(Uppest_Assembly_index).x1;
        else if ((extremum.y == Assembly.segment.at(Uppest_Assembly_index).y2))
            extremum.x = Assembly.segment.at(Uppest_Assembly_index).x2;
        else
            extremum.x = Assembly.segment.at(Uppest_Assembly_index).center_x + hypot(Assembly.segment.at(Uppest_Assembly_index).y2 - Assembly.segment.at(Uppest_Assembly_index).center_y, Assembly.segment.at(Uppest_Assembly_index).x2 - Assembly.segment.at(Uppest_Assembly_index).center_y);
        Silkscreen = Add_Excess_Silkscreen_For_Boarder_Condition(Silkscreen, extremum, Copper_Expanded, 4, Assembly);
    }

    return Silkscreen;
}

vector<Graph> Silkscreen::Add_Excess_Silkscreen_For_Boarder_Condition(vector<Graph> Silkscreen, Point extremum, vector<Graph> Copper_Expanded, int side, Graph Assembly)
{
    int Copper_Expanded_size = Copper_Expanded.size();
    Segment Boarder;
    Boarder.is_line = true;

    for (int i = 0; i < Copper_Expanded_size; i++)
    {
        if (Copper_Expanded.at(i).x_min <= extremum.x && Copper_Expanded.at(i).x_max >= extremum.x && Copper_Expanded.at(i).y_min <= extremum.y && Copper_Expanded.at(i).y_max >= extremum.y) // 在 copper 裡面
        {
            switch (side) // 沿著須處理的邊做一直線，而後與copper圖型取交點
            {
            case 1: // left
            case 2: // right
                Boarder.x1 = Boarder.x2 = extremum.x;
                Boarder.y1 = Copper_Expanded.at(i).y_min - 1;
                Boarder.y2 = Copper_Expanded.at(i).y_max + 1;
                break;
            case 3: // bottom
            case 4: // up
                Boarder.y1 = Boarder.y2 = extremum.y;
                Boarder.x1 = Copper_Expanded.at(i).x_min - 1;
                Boarder.x2 = Copper_Expanded.at(i).x_max + 1;
                break;
            }
            /*
            vector<Segment> Boarder_piece; // 邊界劃過copper的線段
            Boarder_piece = silkscreen_cut_single_copper(Boarder, Copper_Expanded.at(i));
            if (Boarder_piece.size() == 0)
            {
                cout << "error: Boarder_piece.size() == 0" << endl;
                return vector<Segment>();
            }

            vector<Point> Extend_Line_End_Points; // 延伸線段的終點

            int Boarder_piece_size = Boarder_piece.size();
            for (int j = 0; j < Boarder_piece_size; j++) // 將終點轉為點形式
            {
                Point temp;
                temp.x = Boarder_piece.at(j).x1;
                temp.y = Boarder_piece.at(j).y1;
                temp.Next_Arc = false;
                Extend_Line_End_Points.push_back(temp);
                temp.x = Boarder_piece.at(j).x2;
                temp.y = Boarder_piece.at(j).y2;
                temp.Next_Arc = false;
                Extend_Line_End_Points.push_back(temp);
            }
            */
            vector<vector<Point>> Arc_Dots;
            vector<Point> Copper_Dots;
            Arc_Dots = Arc_Optimization(Copper_Expanded.at(i));
            Copper_Dots = Line_to_Point(Copper_Expanded.at(i));

            vector<Point> Boarder_Dots;                 // 在此銅箔上的點
            vector<bool> Boarder_Dots_is_First_Point;   // 在此銅箔上的點是否為第一點
            vector<int> Boarder_Dots_index_in_Assembly; // 在此銅箔上的點在assembly的index
            int Silkscreen_size = Silkscreen.size();

            for (int j = 0; j < Silkscreen_size; j++)
            {
                Point temp;
                temp.x = Silkscreen.at(j).segment.at(0).x1; // 連續線段的第一個點
                temp.y = Silkscreen.at(j).segment.at(0).y1;
                temp.Next_Arc = false;
                if (point_in_polygon(temp, Copper_Dots, Arc_Dots) == true) // 點在 copper 上面
                {
                    Boarder_Dots.push_back(temp);
                    Boarder_Dots_is_First_Point.push_back(true);
                    Boarder_Dots_index_in_Assembly.push_back(j);
                }

                temp.x = Silkscreen.at(j).segment.at(Silkscreen.at(j).segment.size() - 1).x2; // 連續線段的最後一個點
                temp.y = Silkscreen.at(j).segment.at(Silkscreen.at(j).segment.size() - 1).y2;
                temp.Next_Arc = false;
                if (point_in_polygon(temp, Copper_Dots, Arc_Dots) == true) // 點在 copper 上面
                {
                    Boarder_Dots.push_back(temp);
                    Boarder_Dots_is_First_Point.push_back(false);
                    Boarder_Dots_index_in_Assembly.push_back(j);
                }
            }

            int Boarder_Dots_size = Boarder_Dots.size();
            int Boarder_Dots_index; // Boarder_Dots在copper線段中的index

            Graph Fake_Copper_for_Assembly; // 假的銅箔，用來比較點是否在線段上
            Fake_Copper_for_Assembly = Assembly;
            bool Qualified_Dots_Front = false; // 在線段上的點是否符合要求
            bool Qualified_Dots_Back = false;  // 在線段上的點是否符合要求

            vector<Graph> Extended_Silkscreen_Candidate; // 延伸的silkscreen
            Graph Extended_Silkscreen_Front;             // 延伸的silkscreen
            Graph Extended_Silkscreen_Back;              // 延伸的silkscreen
            int Singal_Copper_Expanded_size = Copper_Expanded.at(i).segment.size();

            for (int j = 0; j < Boarder_Dots_size; j++) // 找到點位於copper的哪一個線段上
            {
                for (int k = 0; k < Singal_Copper_Expanded_size; k++)
                {
                    if (Copper_Expanded.at(i).segment.at(k).is_line) // 直線
                    {
                        Point a_vector, b_vector;
                        a_vector.x = Copper_Expanded.at(i).segment.at(k).x1 - Boarder_Dots.at(j).x;
                        a_vector.y = Copper_Expanded.at(i).segment.at(k).y1 - Boarder_Dots.at(j).y;
                        b_vector.y = Copper_Expanded.at(i).segment.at(k).y2 - Boarder_Dots.at(j).y;
                        b_vector.x = Copper_Expanded.at(i).segment.at(k).x2 - Boarder_Dots.at(j).x;
                        if (V_Op.cross(a_vector, b_vector) == 0 && V_Op.dot(a_vector, b_vector) <= 0) // 點在線段上
                        {
                            Boarder_Dots_index = k;
                            break;
                        }
                    }
                    else // 圓弧
                    {
                        float theta = atan2(Boarder_Dots.at(j).y - Copper_Expanded.at(i).segment.at(k).center_y, Boarder_Dots.at(j).x - Copper_Expanded.at(i).segment.at(k).center_x);
                        if (Point_Inside_Arc(theta, Copper_Expanded.at(i).segment.at(k).detail.theta_1, Copper_Expanded.at(i).segment.at(k).detail.theta_2, Copper_Expanded.at(i).segment.at(k).is_CCW))
                        {
                            Boarder_Dots_index = k;
                            break;
                        }
                    }
                }
                bool first_line = true;

                for (int k = Boarder_Dots_index;; (k) ? k-- : k = Singal_Copper_Expanded_size - 1) // 往前找
                {
                    Point First;
                    Point Second;
                    First.x = Copper_Expanded.at(i).segment.at(k).x1;
                    First.y = Copper_Expanded.at(i).segment.at(k).y1;
                    Second.x = Copper_Expanded.at(i).segment.at(k).x2;
                    Second.y = Copper_Expanded.at(i).segment.at(k).y2;

                    Point Boarder_first, Boarder_second;
                    Boarder_first.x = Boarder.x1;
                    Boarder_first.y = Boarder.y1;
                    Boarder_second.x = Boarder.x2;
                    Boarder_second.y = Boarder.y2;

                    Point Extend_End_Point;
                    vector<Point> Extend_End_Point_for_Copper_Arc;

                    if (first_line == true)
                    {
                        Second.x = Boarder_Dots.at(j).x;
                        Second.y = Boarder_Dots.at(j).y;
                    }
                    else if (first_line == false && k == Boarder_Dots_index)
                    {
                        First.x = Boarder_Dots.at(j).x;
                        First.y = Boarder_Dots.at(j).y;
                    }

                    if (Copper_Expanded.at(i).segment.at(k).is_line)
                    {
                        Extend_End_Point = intersection(First, Second, Boarder_first, Boarder_second);
                    }
                    else
                    {
                        Extend_End_Point_for_Copper_Arc = intersection_between_line_and_arc(Copper_Expanded.at(i).segment.at(k), Boarder_first, Boarder_second);
                        if (Extend_End_Point_for_Copper_Arc.size() > 1)
                        {
                            Extend_End_Point = (hypot(Extend_End_Point_for_Copper_Arc.at(0).x - Second.x, Extend_End_Point_for_Copper_Arc.at(0).y - Second.y) < hypot(Extend_End_Point_for_Copper_Arc.at(1).x - Second.x, Extend_End_Point_for_Copper_Arc.at(1).y - Second.y)) ? Extend_End_Point_for_Copper_Arc.at(0) : Extend_End_Point_for_Copper_Arc.at(1);
                        }
                        else if (Extend_End_Point_for_Copper_Arc.size() == 1)
                            Extend_End_Point = Extend_End_Point_for_Copper_Arc.at(0);
                    }

                    Segment temp;
                    temp = Copper_Expanded.at(i).segment.at(k);
                    if (!temp.is_line)
                    {
                        if (temp.x1 != Second.x || temp.y1 != Second.y)
                        {
                            temp.detail.theta_1 = atan2(Second.y - temp.center_y, Second.x - temp.center_x);
                        }
                        if (temp.x2 != First.x || temp.y2 != First.y)
                        {
                            temp.detail.theta_2 = atan2(First.y - temp.center_y, First.x - temp.center_x);
                        }
                    }
                    temp.x1 = Second.x;
                    temp.y1 = Second.y;
                    temp.x2 = First.x;
                    temp.y2 = First.y;

                    if (Extend_End_Point.x != INFINITY && Extend_End_Point.y != INFINITY) // 找到延伸至極值的線段
                    {
                        Qualified_Dots_Front = true;
                        temp.x2 = Extend_End_Point.x;
                        temp.y2 = Extend_End_Point.y;
                        Extended_Silkscreen_Front.segment.push_back(temp);
                        break;
                    }

                    if (!silkscreen_cut_single_copper(temp, Fake_Copper_for_Assembly).segment.empty()) // 線段經過assembly的內部
                    {
                        Qualified_Dots_Front = false;
                        break;
                    }
                    Extended_Silkscreen_Front.segment.push_back(temp);
                    first_line = false;
                }

                first_line = true;

                for (int k = Boarder_Dots_index;; (k == Singal_Copper_Expanded_size) ? k = 0 : k++) // 往後找
                {
                    Point First;
                    Point Second;
                    First.x = Copper_Expanded.at(i).segment.at(k).x1;
                    First.y = Copper_Expanded.at(i).segment.at(k).y1;
                    Second.x = Copper_Expanded.at(i).segment.at(k).x2;
                    Second.y = Copper_Expanded.at(i).segment.at(k).y2;

                    Point Boarder_first, Boarder_second;
                    Boarder_first.x = Boarder.x1;
                    Boarder_first.y = Boarder.y1;
                    Boarder_second.x = Boarder.x2;
                    Boarder_second.y = Boarder.y2;

                    Point Extend_End_Point;
                    vector<Point> Extend_End_Point_for_Copper_Arc;

                    if (first_line == true)
                    {
                        First.x = Boarder_Dots.at(j).x;
                        First.y = Boarder_Dots.at(j).y;
                    }
                    else if (first_line == false && k == Boarder_Dots_index)
                    {
                        Second.x = Boarder_Dots.at(j).x;
                        Second.y = Boarder_Dots.at(j).y;
                    }

                    if (Copper_Expanded.at(i).segment.at(k).is_line)
                    {
                        Extend_End_Point = intersection(First, Second, Boarder_first, Boarder_second);
                    }
                    else
                    {
                        Extend_End_Point_for_Copper_Arc = intersection_between_line_and_arc(Copper_Expanded.at(i).segment.at(k), Boarder_first, Boarder_second);
                        if (Extend_End_Point_for_Copper_Arc.size() > 1)
                        {
                            Extend_End_Point = (hypot(Extend_End_Point_for_Copper_Arc.at(0).x - First.x, Extend_End_Point_for_Copper_Arc.at(0).y - First.y) < hypot(Extend_End_Point_for_Copper_Arc.at(1).x - First.x, Extend_End_Point_for_Copper_Arc.at(1).y - First.y)) ? Extend_End_Point_for_Copper_Arc.at(0) : Extend_End_Point_for_Copper_Arc.at(1);
                        }
                        else if (Extend_End_Point_for_Copper_Arc.size() == 1)
                            Extend_End_Point = Extend_End_Point_for_Copper_Arc.at(0);
                    }

                    Segment temp;
                    temp = Copper_Expanded.at(i).segment.at(k);

                    if (!temp.is_line)
                    {
                        if (temp.x1 != First.x || temp.y1 != First.y)
                        {
                            temp.detail.theta_1 = atan2(First.y - temp.center_y, First.x - temp.center_x);
                        }
                        if (temp.x2 != Second.x || temp.y2 != Second.y)
                        {
                            temp.detail.theta_2 = atan2(Second.y - temp.center_y, Second.x - temp.center_x);
                        }
                    }
                    temp.x1 = First.x;
                    temp.y1 = First.y;
                    temp.x2 = Second.x;
                    temp.y2 = Second.y;

                    if (Extend_End_Point.x != INFINITY && Extend_End_Point.y != INFINITY) // 找到延伸至極值的線段
                    {
                        Qualified_Dots_Back = true;
                        temp.x2 = Extend_End_Point.x;
                        temp.y2 = Extend_End_Point.y;
                        Extended_Silkscreen_Back.segment.push_back(temp);
                        break;
                    }

                    if (!silkscreen_cut_single_copper(temp, Fake_Copper_for_Assembly).segment.empty()) // 線段經過assembly的內部
                    {
                        Qualified_Dots_Back = false;
                        break;
                    }
                    Extended_Silkscreen_Back.segment.push_back(temp);
                    first_line = false;
                }

                if (Qualified_Dots_Front == true && Qualified_Dots_Back == true)
                {
                    // 比較長度，選擇較短者
                }
                else if (Qualified_Dots_Front == true)
                {
                    Extended_Silkscreen_Candidate.push_back(Extended_Silkscreen_Front);
                }
                else if (Qualified_Dots_Back == true)
                {
                    Extended_Silkscreen_Candidate.push_back(Extended_Silkscreen_Back);
                }
                else
                {
                    Extended_Silkscreen_Candidate.push_back(Graph());
                }
            }
            float min_length = INFINITY;
            float temp_length;
            vector<Segment> Extended_Silkscreen;
            int Extended_Silkscreen_index = 0;                    // 第幾個延伸線段
            int Extended_Silkscreen_index_in_Original_Silkscreen; // 延伸線段需接在第幾個原始線段中
            for (int j = 0; j < Boarder_Dots_size; j++)
            {
                // 比較各合法線段長度，選擇較短者
                temp_length = Calculate_Silkscreen_length(Extended_Silkscreen_Candidate.at(j));
                if (temp_length < min_length)
                {
                    min_length = temp_length;
                    Extended_Silkscreen = Extended_Silkscreen_Candidate.at(j).segment;
                    Extended_Silkscreen_index = j;
                    Extended_Silkscreen_index_in_Original_Silkscreen = Boarder_Dots_index_in_Assembly.at(j);
                }
            }
            vector<Segment> Inverted_Extended_Silkscreen;
            if (Boarder_Dots_is_First_Point.at(Extended_Silkscreen_index)) // 斷點是第一點
            {
                for (int j = Extended_Silkscreen.size() - 1; j >= 0; j--)
                {
                    swap(Extended_Silkscreen.at(j).x1, Extended_Silkscreen.at(j).x2);
                    swap(Extended_Silkscreen.at(j).y1, Extended_Silkscreen.at(j).y2);
                    swap(Extended_Silkscreen.at(j).detail.theta_1, Extended_Silkscreen.at(j).detail.theta_2);
                    Inverted_Extended_Silkscreen.push_back(Extended_Silkscreen.at(j));
                }
                Silkscreen.at(Extended_Silkscreen_index_in_Original_Silkscreen).segment.insert(Silkscreen.at(Extended_Silkscreen_index_in_Original_Silkscreen).segment.begin(), Inverted_Extended_Silkscreen.begin(), Inverted_Extended_Silkscreen.end());
            }
            else
            {
                Silkscreen.at(Extended_Silkscreen_index_in_Original_Silkscreen).segment.insert(Silkscreen.at(Extended_Silkscreen_index_in_Original_Silkscreen).segment.end(), Extended_Silkscreen.begin(), Extended_Silkscreen.end());
            }
        }
    }
    return Silkscreen;
}

vector<vector<Point>> Silkscreen::Arc_Optimization(Graph Assembly)
{
    int Assembly_size = Assembly.segment.size();
    vector<Point> Dots_of_Arc;
    vector<vector<Point>> vector_of_Arc;
    for (int i = 0; i < Assembly_size; i++)
    {
        if (!Assembly.segment.at(i).is_line) // the Segment is arc
        {
            Dots_of_Arc.clear();
            Dots_of_Arc = Arc_to_Poly(Assembly.segment.at(i));
            vector_of_Arc.push_back(Dots_of_Arc);
        }
    }
    return vector_of_Arc;
}

vector<Point> Silkscreen::Arc_to_Poly(Segment Arc)
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

vector<Point> Silkscreen::Line_to_Point(const Graph Assembly) // 將線段切割成點
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

bool Silkscreen::point_in_polygon(Point t, vector<Point> Assembly_Point, vector<vector<Point>> Arc_Points) // 運用射線法判斷點在圖形內外
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

float Silkscreen::interpolate_x(float y, Point p1, Point p2) // 待測點與圖形邊界交會的x值
{
    if (p1.y == p2.y)
        return p1.x;
    return p1.x + (p2.x - p1.x) * (y - p1.y) / (p2.y - p1.y);
}

bool Silkscreen::Point_Inside_Arc(float Point_Theta, float Arc_Theta1, float Arc_Theta2, bool is_CounterClockwise) // 確認點是否在圓弧內，已確認在該圓弧所屬的園內 (counterclockwise)
{
    if (!is_CounterClockwise)
    {
        swap(Arc_Theta1, Arc_Theta2);
    }
    if (Arc_Theta1 == Arc_Theta2) // 完整的圓，一定是在圓弧內
        return true;
    if (Arc_Theta1 > Arc_Theta2)
    {
        if (Point_Theta >= Arc_Theta1 || Point_Theta <= Arc_Theta2)
            return true;
        else
            return false;
    }
    else
    {
        if (Point_Theta >= Arc_Theta1 && Point_Theta <= Arc_Theta2)
            return true;
        else
            return false;
    }
}

Point Silkscreen::intersection(Point a1, Point a2, Point b1, Point b2)
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

    if (In_Between_Lines(intersect, a1, a2) && In_Between_Lines(intersect, b1, b2))
        return intersect;
    else
    {
        s.x = s.y = INFINITY;
        return s;
    }
}

bool Silkscreen::In_Between_Lines(Point test, Point first, Point last) // safe
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

vector<Point> Silkscreen::intersection_between_line_and_arc(Segment Arc, Point Line_First_Point, Point Line_Second_Point)
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
            if (Point_Inside_Arc(Point_Theta, Arc.detail.theta_1, Arc.detail.theta_2, Arc.is_CCW))
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
            if (Point_Inside_Arc(Point_Theta, Arc.detail.theta_1, Arc.detail.theta_2, Arc.is_CCW))
                Intersection_Points.push_back(P2);
        }
        return Intersection_Points;
    }
    return vector<Point>();
}

float Silkscreen::Calculate_Silkscreen_length(const Graph &Silkscreen)
{
    float len;
    size_t size = Silkscreen.segment.size();
    for (size_t i = 0; i < size; i++)
    {
        if (Silkscreen.segment.at(i).is_line)
            len += hypot(Silkscreen.segment.at(i).x2 - Silkscreen.segment.at(i).x1, Silkscreen.segment.at(i).y2 - Silkscreen.segment.at(i).y1);
        else // arc
        {
            float circumference = hypot(Silkscreen.segment.at(i).x2 - Silkscreen.segment.at(i).center_x, Silkscreen.segment.at(i).y2 - Silkscreen.segment.at(i).center_y);
            float angle_1, angle_2, angle_between;
            angle_1 = Silkscreen.segment.at(i).detail.theta_1;
            angle_2 = Silkscreen.segment.at(i).detail.theta_2;
            if (Silkscreen.segment.at(i).is_CCW)
                swap(angle_1, angle_2);
            angle_between = angle_1 - angle_2;
            if (angle_between <= 0)
                angle_between += 2 * PI;
            float partial_circumference = circumference * angle_between;

            len += partial_circumference;
        }
    }
    return len;
}
// -----------------------------------End fit_boarder_condition and its dependencies -----------------------------------