//Silkscreen.cpp 
//Implementation of Silkscreen Buffer and Modify the Silkscreen
#include <bits/stdc++.h>
#include "Silkscreen.h"

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

    float Assembly_x_min = Assembly.at(0).x_min;
    float Assembly_x_max = Assembly.at(0).x_max;
    float Assembly_y_min = Assembly.at(0).y_min;
    float Assembly_y_max = Assembly.at(0).y_max;

    int Uppest_Assembly_index = 0;   // assembly 最上面的線
    int Lowest_Assembly_index = 0;   // assembly 最下面的線
    int Leftest_Assembly_index = 0;  // assembly 最左邊的線
    int Rightest_Assembly_index = 0; // assembly 最右邊的線

    float Silkscreen_x_min = Silkscreen.at(0).at(0).x_min;
    float Silkscreen_x_max = Silkscreen.at(0).at(0).x_max;
    float Silkscreen_y_min = Silkscreen.at(0).at(0).y_min;
    float Silkscreen_y_max = Silkscreen.at(0).at(0).y_max;

    vector<vector<Segment>> Silkscreen_fit_Condition;

    for (int i = 0; i < Assembly_size; i++) // 找assembly極值
    {
        if (Assembly.at(i).x_min < Assembly_x_min)
        {
            Assembly_x_min = Assembly.at(i).x_min;
            Leftest_Assembly_index = i;
        }
        if (Assembly.at(i).x_max > Assembly_x_max)
        {
            Assembly_x_max = Assembly.at(i).x_max;
            Rightest_Assembly_index = i;
        }
        if (Assembly.at(i).y_min < Assembly_y_min)
        {
            Assembly_y_min = Assembly.at(i).y_min;
            Lowest_Assembly_index = i;
        }
        if (Assembly.at(i).y_max > Assembly_y_max)
        {
            Assembly_y_max = Assembly.at(i).y_max;
            Uppest_Assembly_index = i;
        }
    }
    for (int i = 0; i < Silkscreen_size; i++) // 找絲印極值
    {
        Silkscreen_piece_size = Silkscreen.at(i).size();
        for (int j = 0; j < Silkscreen_piece_size; j++)
        {
            if (Silkscreen.at(i).at(j).x_min < Silkscreen_x_min)
            {
                Silkscreen_x_min = Silkscreen.at(i).at(j).x_min;
            }
            if (Silkscreen.at(i).at(j).x_max > Silkscreen_x_max)
            {
                Silkscreen_x_max = Silkscreen.at(i).at(j).x_max;
            }
            if (Silkscreen.at(i).at(j).y_min < Silkscreen_y_min)
            {
                Silkscreen_y_min = Silkscreen.at(i).at(j).y_min;
            }
            if (Silkscreen.at(i).at(j).y_max > Silkscreen_y_max)
            {
                Silkscreen_y_max = Silkscreen.at(i).at(j).y_max;
            }
        }
    }

    Point extremum;

    if (Silkscreen_x_min > Assembly_x_min) // 左方沒包住
    {
        extremum.x = Assembly.at(Leftest_Assembly_index).x_min;
        if (extremum.x == Assembly.at(Leftest_Assembly_index).x1)
            extremum.y = Assembly.at(Leftest_Assembly_index).y1;
        else if ((extremum.x == Assembly.at(Leftest_Assembly_index).x2))
            extremum.y = Assembly.at(Leftest_Assembly_index).y2;
        else
            extremum.y = Assembly.at(Leftest_Assembly_index).center_y - hypot(Assembly.at(Leftest_Assembly_index).x2 - Assembly.at(Leftest_Assembly_index).center_x, Assembly.at(Leftest_Assembly_index).y2 - Assembly.at(Leftest_Assembly_index).center_x);
        Silkscreen = Add_Excess_Silkscreen_For_Boarder_Condition(Silkscreen, extremum, Copper_Expanded, 1, Assembly);
    }
    if (Silkscreen_x_max < Assembly_x_max) // 右方沒包住
    {
        extremum.x = Assembly.at(Rightest_Assembly_index).x_max;
        if (extremum.x == Assembly.at(Rightest_Assembly_index).x1)
            extremum.y = Assembly.at(Rightest_Assembly_index).y1;
        else if ((extremum.x == Assembly.at(Rightest_Assembly_index).x2))
            extremum.y = Assembly.at(Rightest_Assembly_index).y2;
        else
            extremum.y = Assembly.at(Rightest_Assembly_index).center_y + hypot(Assembly.at(Rightest_Assembly_index).x2 - Assembly.at(Rightest_Assembly_index).center_x, Assembly.at(Rightest_Assembly_index).y2 - Assembly.at(Rightest_Assembly_index).center_x);
        Silkscreen = Add_Excess_Silkscreen_For_Boarder_Condition(Silkscreen, extremum, Copper_Expanded, 2, Assembly);
    }
    if (Silkscreen_y_min > Assembly_y_min) // 下方沒包住
    {
        extremum.y = Assembly.at(Lowest_Assembly_index).y_min;
        if (extremum.y == Assembly.at(Lowest_Assembly_index).y1)
            extremum.x = Assembly.at(Lowest_Assembly_index).x1;
        else if ((extremum.y == Assembly.at(Lowest_Assembly_index).y2))
            extremum.x = Assembly.at(Lowest_Assembly_index).x2;
        else
            extremum.x = Assembly.at(Lowest_Assembly_index).center_x - hypot(Assembly.at(Lowest_Assembly_index).y2 - Assembly.at(Lowest_Assembly_index).center_y, Assembly.at(Lowest_Assembly_index).x2 - Assembly.at(Lowest_Assembly_index).center_y);
        Silkscreen = Add_Excess_Silkscreen_For_Boarder_Condition(Silkscreen, extremum, Copper_Expanded, 3, Assembly);
    }
    if (Silkscreen_y_max < Assembly_y_max) // 上方沒包住
    {
        extremum.y = Assembly.at(Uppest_Assembly_index).y_max;
        if (extremum.y == Assembly.at(Uppest_Assembly_index).y1)
            extremum.x = Assembly.at(Uppest_Assembly_index).x1;
        else if ((extremum.y == Assembly.at(Uppest_Assembly_index).y2))
            extremum.x = Assembly.at(Uppest_Assembly_index).x2;
        else
            extremum.x = Assembly.at(Uppest_Assembly_index).center_x + hypot(Assembly.at(Uppest_Assembly_index).y2 - Assembly.at(Uppest_Assembly_index).center_y, Assembly.at(Uppest_Assembly_index).x2 - Assembly.at(Uppest_Assembly_index).center_y);
        Silkscreen = Add_Excess_Silkscreen_For_Boarder_Condition(Silkscreen, extremum, Copper_Expanded, 4, Assembly);
    }

    return Silkscreen;
}

vector<vector<Segment>> Silkscreen::Add_Excess_Silkscreen_For_Boarder_Condition(vector<vector<Segment>> Silkscreen, Point extremum, vector<Copper> Copper_Expanded, int side, vector<Segment> Assembly)
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
            Arc_Dots = Arc_Optimization(Copper_Expanded.at(i).segment);
            Copper_Dots = Line_to_Point(Copper_Expanded.at(i).segment);

            vector<Point> Boarder_Dots;                 // 在此銅箔上的點
            vector<bool> Boarder_Dots_is_First_Point;   // 在此銅箔上的點是否為第一點
            vector<int> Boarder_Dots_index_in_Assembly; // 在此銅箔上的點在assembly的index
            int Silkscreen_size = Silkscreen.size();

            for (int j = 0; j < Silkscreen_size; j++)
            {
                Point temp;
                temp.x = Silkscreen.at(j).at(0).x1; // 連續線段的第一個點
                temp.y = Silkscreen.at(j).at(0).y1;
                temp.Next_Arc = false;
                if (point_in_polygon(temp, Copper_Dots, Arc_Dots) == true) // 點在 copper 上面
                {
                    Boarder_Dots.push_back(temp);
                    Boarder_Dots_is_First_Point.push_back(true);
                    Boarder_Dots_index_in_Assembly.push_back(j);
                }

                temp.x = Silkscreen.at(j).at(Silkscreen.at(j).size() - 1).x2; // 連續線段的最後一個點
                temp.y = Silkscreen.at(j).at(Silkscreen.at(j).size() - 1).y2;
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

            Copper Fake_Copper_for_Assembly; // 假的銅箔，用來比較點是否在線段上
            Fake_Copper_for_Assembly.segment = Assembly;
            bool Qualified_Dots_Front = false; // 在線段上的點是否符合要求
            bool Qualified_Dots_Back = false;  // 在線段上的點是否符合要求

            vector<vector<Segment>> Extended_Silkscreen_Candidate; // 延伸的silkscreen
            vector<Segment> Extended_Silkscreen_Front;             // 延伸的silkscreen
            vector<Segment> Extended_Silkscreen_Back;              // 延伸的silkscreen
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
                        if (cross(a_vector, b_vector) == 0 && dot(a_vector, b_vector) <= 0) // 點在線段上
                        {
                            Boarder_Dots_index = k;
                            break;
                        }
                    }
                    else // 圓弧
                    {
                        float theta = atan2(Boarder_Dots.at(j).y - Copper_Expanded.at(i).segment.at(k).center_y, Boarder_Dots.at(j).x - Copper_Expanded.at(i).segment.at(k).center_x);
                        if (Point_Inside_Arc(theta, Copper_Expanded.at(i).segment.at(k).theta_1, Copper_Expanded.at(i).segment.at(k).theta_2, Copper_Expanded.at(i).segment.at(k).direction))
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
                            temp.theta_1 = atan2(Second.y - temp.center_y, Second.x - temp.center_x);
                        }
                        if (temp.x2 != First.x || temp.y2 != First.y)
                        {
                            temp.theta_2 = atan2(First.y - temp.center_y, First.x - temp.center_x);
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
                        Extended_Silkscreen_Front.push_back(temp);
                        break;
                    }

                    if (!silkscreen_cut_single_copper(temp, Fake_Copper_for_Assembly).empty()) // 線段經過assembly的內部
                    {
                        Qualified_Dots_Front = false;
                        break;
                    }
                    Extended_Silkscreen_Front.push_back(temp);
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
                            temp.theta_1 = atan2(First.y - temp.center_y, First.x - temp.center_x);
                        }
                        if (temp.x2 != Second.x || temp.y2 != Second.y)
                        {
                            temp.theta_2 = atan2(Second.y - temp.center_y, Second.x - temp.center_x);
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
                        Extended_Silkscreen_Back.push_back(temp);
                        break;
                    }

                    if (!silkscreen_cut_single_copper(temp, Fake_Copper_for_Assembly).empty()) // 線段經過assembly的內部
                    {
                        Qualified_Dots_Back = false;
                        break;
                    }
                    Extended_Silkscreen_Back.push_back(temp);
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
                    Extended_Silkscreen_Candidate.push_back(vector<Segment>());
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
                    Extended_Silkscreen = Extended_Silkscreen_Candidate.at(j);
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
                    swap(Extended_Silkscreen.at(j).theta_1, Extended_Silkscreen.at(j).theta_2);
                    Inverted_Extended_Silkscreen.push_back(Extended_Silkscreen.at(j));
                }
                Silkscreen.at(Extended_Silkscreen_index_in_Original_Silkscreen).insert(Silkscreen.at(Extended_Silkscreen_index_in_Original_Silkscreen).begin(), Inverted_Extended_Silkscreen.begin(), Inverted_Extended_Silkscreen.end());
            }
            else
            {
                Silkscreen.at(Extended_Silkscreen_index_in_Original_Silkscreen).insert(Silkscreen.at(Extended_Silkscreen_index_in_Original_Silkscreen).end(), Extended_Silkscreen.begin(), Extended_Silkscreen.end());
            }
        }
    }
    return Silkscreen;
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