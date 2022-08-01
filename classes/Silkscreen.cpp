//Silkscreen.cpp 
//Implementation of Silkscreen Buffer and Modify the Silkscreen
#include <bits/stdc++.h>
#include "Silkscreen.h"
#include "Point.h"
#include "Segment.h"
#include "Graph.h"
#include "Parameter.h"

using namespace std;
//Silkscreen_Assembly not founded.

Graph Silkscreen::Untuned_Silkscreen(Graph Silkscreen_Original, vector<Graph> Coppers) // 未切割的絲印 與 外擴的銅箔
{
    Graph Silkscreen_Cut_Complete; // 切割完成的完整絲印
    Graph Silkscreen_Cut_Part;     // 切割完成的一條絲印
    int Silkscreen_Org_Size = Silkscreen_Original.size();

    for (int i = 0; i < Silkscreen_Org_Size; i++)
    {
        Silkscreen_Cut_Part.segment.clear();
        Silkscreen_Cut_Part = Cut_Silkscreen_by_Copper(Silkscreen_Original.at(i), Coppers);
        int Silkscreen_Cut_Part_Size = Silkscreen_Cut_Part.size();
        for (int j = 0; j < Silkscreen_Cut_Part_Size; j++)
        {
            if (Silkscreen_Cut_Part.at(j).is_line)
            {
                Silkscreen_Cut_Part.at(j).x_min = min(Silkscreen_Cut_Part.at(j).x1, Silkscreen_Cut_Part.at(j).x2);
                Silkscreen_Cut_Part.at(j).x_max = max(Silkscreen_Cut_Part.at(j).x1, Silkscreen_Cut_Part.at(j).x2);
                Silkscreen_Cut_Part.at(j).y_min = min(Silkscreen_Cut_Part.at(j).y1, Silkscreen_Cut_Part.at(j).y2);
                Silkscreen_Cut_Part.at(j).y_max = max(Silkscreen_Cut_Part.at(j).y1, Silkscreen_Cut_Part.at(j).y2);
            }
            else
            {
                Silkscreen_Cut_Part.at(j) = Arc_Boundary_Meas_for_Assembly(Silkscreen_Cut_Part.at(j));
            }
        }
        Silkscreen_Cut_Complete.insert(Silkscreen_Cut_Complete.end(), Silkscreen_Cut_Part.begin(), Silkscreen_Cut_Part.end());
    }
    return Silkscreen_Cut_Complete;
}

vector<vector<Segment>> Silkscreen::Delete_Short_Silkscreen(vector<Segment> Silkscreen)
{
    float len;
    vector<vector<Segment>> All_Continuous;
    All_Continuous = Find_Continuous_Segment(Silkscreen);
    int Continue_size = All_Continuous.size();
    for (int i = 0; i < Continue_size; i++)
    {
        int A_Continuous_Segment_size = All_Continuous.at(i).size();
        len = 0;
        for (int j = 0; j < A_Continuous_Segment_size; j++)
        {
            if (All_Continuous.at(i).at(j).is_line)
            {
                len += hypot(All_Continuous.at(i).at(j).x2 - All_Continuous.at(i).at(j).x1, All_Continuous.at(i).at(j).y2 - All_Continuous.at(i).at(j).y1);
            }
            else // arc
            {
                float circumference = hypot(All_Continuous.at(i).at(j).x2 - All_Continuous.at(i).at(j).center_x, All_Continuous.at(i).at(j).y2 - All_Continuous.at(i).at(j).center_y);
                float angle_1, angle_2, angle_between;
                angle_1 = All_Continuous.at(i).at(j).theta_1;
                angle_2 = All_Continuous.at(i).at(j).theta_2;
                if (All_Continuous.at(i).at(j).direction)
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
    return All_Continuous;
}

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

                    if (!silkscreen_cut_single_copper(temp, Fake_Copper_for_Assembly).empty()) // 線段經過assembly的內部
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

                    if (!silkscreen_cut_single_copper(temp, Fake_Copper_for_Assembly).empty()) // 線段經過assembly的內部
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

float Silkscreen::Calculate_Silkscreen_length(const Graph& Silkscreen)
{
    float len;
    int size = Silkscreen.size();
    for (int i = 0; i < size; i++)
    {
        if (Silkscreen.at(i).is_line)
            len += hypot(Silkscreen.at(i).x2 - Silkscreen.at(i).x1, Silkscreen.at(i).y2 - Silkscreen.at(i).y1);
        else // arc
        {
            float circumference = hypot(Silkscreen.at(i).x2 - Silkscreen.at(i).center_x, Silkscreen.at(i).y2 - Silkscreen.at(i).center_y);
            float angle_1, angle_2, angle_between;
            angle_1 = Silkscreen.at(i).theta_1;
            angle_2 = Silkscreen.at(i).theta_2;
            if (Silkscreen.at(i).direction)
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