// functions of scorer.h

#include <bits/stdc++.h>
#include "scorer.h"
#include "FileIO.h"
#include "Buffer.h"
#include "Parameter.h"
#include "VectorOp.h"

using namespace std;

//=================function declarations==========================
//int dir(Point, Point, Point);
//double disMin(Point, Point, Point);
Point operator-(Point, Point);
Point operator+(Point, Point);
Point operator*(double, Point);
Point operator/(Point, double);
bool operator==(Point, Point);
bool operator!=(Point, Point);
//Point orth_Cswap(Point);
//Point orth_CCswap(Point);
//double Point_to_Arc_MinDist(Point, Segment);
//vector<Point> intersection_between_CentersLine_and_Arc(Segment, Point);
//bool Line_intersect(Segment, Segment);
//bool On_Arc(Segment, Point);
//bool Concentric_Circle_On_Arc(Segment, Segment);

Scorer::Scorer()
{ // the default constructor, use the defined Q_FILE and A_FILE path
    file.Read_File(INPUT_PATH, OUTPUT_PATH);

    // this->Q_file.open(INPUT_PATH, ios::in);
    // this->A_file.open(OUTPUT_PATH, ios::in);
    // this->open_file();
}

Scorer::Scorer(const char *Ques_File, const char *Ans_File)
{ // modify the Q_FILE and A_FILE path
    file.Read_File(Ques_File, Ans_File);

    // this->Q_file.open(Ques_File, ios::in);
    // this->A_file.open(Ans_File, ios::in);
    // this->open_file();
}

void Scorer::read_file()
{
    // Input_Output A;
    // Q_file.open(INPUT_PATH, ios::in);
    // A_file.open(OUTPUT_PATH, ios::in);
    // string assemblygap_str, coppergap_str, silkscreenlen_str;
    // Q_file >> assemblygap_str >> coppergap_str >> silkscreenlen_str;
    assemblygap = file.File_to_Parameter();
    coppergap = file.File_to_Parameter();
    silkscreenlen = file.File_to_Parameter();

    assembly = file.Read_Assembly();
    copper = file.Read_Copper();
    silkscreen = file.Read_Silkscreen();

    Buffer buffer(assemblygap, coppergap);
    Assembly_push_out = buffer.Assembly_Buffer(assembly);
}

// test passed 2022/07/19
double Scorer::first_quarter() // const vector<Segment> Assembly, const vector<Segment> silkscreen)
{
    float Rectangular_area = 0;       // 絲印標示之座標極限值所構成之矩形面積
    float X_max, Y_max, X_min, Y_min; //絲印座標極限值

    float Y_area = 0; // 零件外觀向外等比拓展Y之面積範圍
    // vector<Segment> Assembly_push_out; 7/10, move to class private area.

    float Answer_1;
    /* calculate Rectangular_area */
    X_max = this->silkscreen.segment.at(0).x1;
    X_min = this->silkscreen.segment.at(0).x2;
    Y_max = this->silkscreen.segment.at(0).y1;
    Y_min = this->silkscreen.segment.at(0).y2;

    for (size_t i = 0; i < this->silkscreen.segment.size(); i++)
    {
        if (silkscreen.segment[i].x2 > X_max)
            X_max = silkscreen.segment[i].x2;
        if (silkscreen.segment[i].x1 > X_max)
            X_max = silkscreen.segment[i].x1;
        if (silkscreen.segment[i].x2 < X_min)
            X_min = silkscreen.segment[i].x2;
        if (silkscreen.segment[i].x1 < X_min)
            X_min = silkscreen.segment[i].x1;

        if (silkscreen.segment[i].y2 > Y_max)
            Y_max = silkscreen.segment[i].y2;
        if (silkscreen.segment[i].y1 > Y_max)
            Y_max = silkscreen.segment[i].y1;
        if (silkscreen.segment[i].y2 < Y_min)
            Y_min = silkscreen.segment[i].y2;
        if (silkscreen.segment[i].y1 < Y_min)
            Y_min = silkscreen.segment[i].y1;
    }
    Rectangular_area = abs((X_max - X_min) * (Y_max - Y_min));

    /* calculate Y_area*/
    vector<Point> Assembly_push_out_points;
    vector<vector<Point>> Arc_Dots;
    Assembly_push_out_points = graph_op.Line_to_Point(Assembly_push_out);
    Arc_Dots = graph_op.Arc_Optimization(Assembly_push_out);

    float total_area = 0;
    size_t i;
    size_t j = i - 1;
    for (i = 0; i < Assembly_push_out.segment.size(); i++)
    {
        if (i == 0)
            j = Assembly_push_out.segment.size() - 1;
        else
            j = i - 1;
        total_area += (Assembly_push_out.segment[j].x1 + Assembly_push_out.segment[i].x1) * (Assembly_push_out.segment[j].y1 - Assembly_push_out.segment[i].y1);
    }
    // Law of cosines
    Point center_of_circle;
    Point point_1;
    Point point_2;
    bool outside;
    float radius = 0; // Law of cosines
    float c = 0;      // Law of cosines
    double theta = 0; // Law of cosines

    float s; // heron formula

    float cut_area;
    float testing_variable = 0;
    for (i = 0; i < Assembly_push_out.segment.size(); i++)
    {
        if (!Assembly_push_out.segment.at(i).is_line)
        {
            center_of_circle.x = Assembly_push_out.segment.at(i).center_x;
            center_of_circle.y = Assembly_push_out.segment.at(i).center_y;
            point_1.x = Assembly_push_out.segment.at(i).x1;
            point_1.y = Assembly_push_out.segment.at(i).y1;
            point_2.x = Assembly_push_out.segment.at(i).x2;
            point_2.y = Assembly_push_out.segment.at(i).y2;

            radius = POINT::dist(center_of_circle, point_1);
            c = POINT::dist(point_1, point_2);
            s = (radius + radius + c) / 2;
            testing_variable = (2 * (radius * radius) - c * c) / (2 * radius * radius);
            theta = acos(testing_variable); // Law of cosines

            cut_area = radius * radius * theta / 2 - sqrt(s * (s - radius) * (s - radius) * (s - c)); // last part is heron formula

            outside = !point_op.point_in_polygon(center_of_circle, Assembly_push_out_points, Arc_Dots);
            if (outside)
                total_area -= cut_area;
            else
                total_area += cut_area;
        }
    }
    total_area /= 2; //修正項
    Answer_1 = (2 - Rectangular_area / (total_area)) * 0.25;

    if (ShowDetail)
    {
        cout << "First Part: ";
        if (total_area >= Rectangular_area)
        {
            cout << "FAIL" << endl;
            cout << "Reason: first quarter wrong total_area >= Rectangular_area " << endl
                 << endl;
        }
        else
        {
            cout << "PASS" << endl
                 << endl;
        }

        cout << "Score:";
        if (Answer_1 > 0.25)
            cout << 0.25 << endl
                 << "over 0.25\n";
        else
            cout << Answer_1
                 << endl
                 << endl;
        cout << "Rectangular_area:" << Rectangular_area << endl;
        cout << "total_area:" << total_area << endl
             << endl
             << endl;
    }

    return (Answer_1 > 0.25) ? 0.25 : Answer_1; //大於0.25只算0.25
}

// finish verification, done on 2022/7/9
double Scorer::second_quarter() // const vector<Segment>Assembly, const vector<Segment> silkscreen)
{
    double part_1{0}, part_2{0}, Second_Score{0}, total_perimeter{0}, total_silkscreen{0};
    int assembly_line{0}, assembly_arc{0}, silk_line{0}, silk_arc{0};
    double number_diff{0};
    // First Part
    // notice!! the buffer of the assembly, not sure it is as same as description.
    // calculate the Perimeter of assembly
    double length{0}, length_x{0}, length_y{0}, radius{0};
    for (size_t i = 0; i < Assembly_push_out.segment.size(); i++)
    {
        length = 0;
        if (Assembly_push_out.segment[i].is_line) // line
        {
            length_x = Assembly_push_out.segment[i].x2 - Assembly_push_out.segment[i].x1;
            length_y = Assembly_push_out.segment[i].y2 - Assembly_push_out.segment[i].y1;
            length = hypot(length_x, length_y);
        }
        else // arc
        {
            length_x = Assembly_push_out.segment[i].center_x - Assembly_push_out.segment[i].x1;
            length_y = Assembly_push_out.segment[i].center_y - Assembly_push_out.segment[i].y1;
            radius = hypot(length_x, length_y);
            length = radius * Arc_Degree(Assembly_push_out.segment[i]);
        }
        total_perimeter += length;
    }
    // read the Answer Silkscreen and count the number of Line and Arc.
    for (size_t i = 0; i < silkscreen.segment.size(); i++)
    {
        length = 0;
        if (silkscreen.segment[i].is_line)
        {
            length_x = silkscreen.segment[i].x2 - silkscreen.segment[i].x1;
            length_y = silkscreen.segment[i].y2 - silkscreen.segment[i].y1;
            length = hypot(length_x, length_y);
            silk_line += 1;
        }
        else
        {
            length_x = silkscreen.segment[i].center_x - silkscreen.segment[i].x1;
            length_y = silkscreen.segment[i].center_y - silkscreen.segment[i].y1;
            radius = hypot(length_x, length_y);
            length = radius * Arc_Degree(silkscreen.segment[i]);
            silk_arc += 1;
        }
        total_silkscreen += length;
    }
    // Second_part
    // count the number of assembly Line and Arc
    for (size_t i = 0; i < assembly.segment.size(); i++)
    {
        if (assembly.segment[i].is_line)
        {
            assembly_line += 1;
        }
        else
        {
            assembly_arc += 1;
        }
    }

    number_diff = (double)(abs((assembly_arc - silk_arc)) + abs((assembly_line - silk_line)));
    part_1 = ((2 - (total_silkscreen / total_perimeter)) > 1) ? 1 : (2 - (total_silkscreen / total_perimeter));
    part_2 = (1 - (number_diff / (double)(assembly.segment.size() + this->copper.size())) > 1) ? 1 : (1 - (number_diff / (double)(assembly.segment.size() + this->copper.size())));
    Second_Score = part_1 * 0.15 + part_2 * 0.10;
    // print the result of second quarter
    if (ShowDetail)
    {
        cout << "Second Part: No Restriction" << endl
             << endl;
        cout << "Score: " << Second_Score << endl
             << endl;
        cout << "Part_1 score: " << part_1 << endl
             << "Part_2 Score: " << part_2 << endl
             << endl
             << endl;
    }
    return Second_Score;
}

double Scorer::Arc_Degree(const Segment &S1)
{
    double degree{0};
    if (!S1.is_line)
    {
        degree = abs(S1.detail.theta_1 - S1.detail.theta_2);
        if (S1.detail.theta_1 > 0 && S1.detail.theta_2 < 0 && S1.is_CCW)
        {
            degree = 2 * PI - degree;
        }
        else
        {
            if (S1.detail.theta_1 < 0 && S1.detail.theta_2 > 0 && !S1.is_CCW)
                degree = 2 * PI - degree;
        }
    }
    return degree;
}

double Scorer::third_quarter() // const vector<vector<Segment>> copper, const vector<Segment> silkscreen)
{
    float L_copper = coppergap;
    double min_distance{0}, min_dist_result{0}, min_copper_distance{0}, min_copper_temp{0};
    double min_tmp;
    double min_distance_sum;

    size_t continue_seg{0};
    int continue_seg_count{0};
    double T_copper;
    double Third_Score;
    Point A1, A2, B1, B2; // A,B兩線段
    double rA = 0;
    double rB = 0;
    bool min_copper_used = false;
    Point circle_center_A, circle_center_B;
    vector<Point> A_ps, S_ps;

    bool pass_monitor = true;

    min_distance_sum = 0;

    for (size_t i = 0; i < silkscreen.segment.size(); i++)
    {
        // count the continue_silkscreen
        if (++continue_seg_count >= continue_num[continue_seg])
        {
            continue_seg++;
            continue_seg_count = 0;
        }
        // skip the silkscreen doesn't have edge points.
        if (continue_seg_count != 0 && continue_seg_count != 1)
        {
            continue;
        }

        A1.x = silkscreen.segment.at(i).x1;
        A1.y = silkscreen.segment.at(i).y1;
        A2.x = silkscreen.segment.at(i).x2;
        A2.y = silkscreen.segment.at(i).y2;
        rA = POINT::dist(A1, circle_center_A);

        for (size_t j = 0; j < copper.size(); j++)
        {
            for (size_t k = 0; k <copper.at(j).segment.size(); k++)
            {
                B1.x = copper.at(j).segment.at(k).x1;
                B1.y = copper.at(j).segment.at(k).y1;
                B2.x = copper.at(j).segment.at(k).x2;
                B2.y = copper.at(j).segment.at(k).y2;
                circle_center_B.x = copper.at(j).segment.at(k).center_x;
                circle_center_B.y = copper.at(j).segment.at(k).center_y;
                rB = POINT::dist(B1, circle_center_B);
                if (dir(A1, A2, B1) * dir(A1, A2, B2) <= 0 && dir(B1, B2, A1) * dir(B1, B2, A2) <= 0) //兩線段相交, 距離為0
                    min_distance = 0;

                if (silkscreen.segment.at(i).is_line && copper.at(j).segment.at(k).is_line)//Silkscreen:line, Copper Segment: lines.
                {
                    if (dir(A1, A2, B1) * dir(A1, A2, B2) <= 0 && dir(B1, B2, A1) * dir(B1, B2, A2) <= 0) //兩線段相交, 距離為0
                        min_distance = 0;
                    else //如不相交, 最短距離為四個端點中到另一條線段距離的最小值
                        min_distance = min(min(min(disMin(A1, A2, B1), disMin(A1, A2, B2)), disMin(B1, B2, A1)), disMin(B1, B2, A2));
                }
                else if (silkscreen.segment.at(i).is_line && !copper.at(j).segment.at(k).is_line)//Silkscreen:line, Copper Segment: arc.
                {
                    min_distance = min(min(min(disMin(A1, A2, B1), disMin(A1, A2, B2)), Point_to_Arc_MinDist(A1, copper.at(j).segment.at(k))), Point_to_Arc_MinDist(A2, copper.at(j).segment.at(k)));

                    if (disMin(A1, A2, circle_center_B) > rB)
                    {
                        A_ps = intersection_between_CentersLine_and_Arc(copper.at(j).segment.at(k), circle_center_B + VectorOp::orth_Cswap(A1 - A2));
                        for (size_t i = 0; i < A_ps.size(); i++)
                        {
                            if (i == 0)
                            {
                                min_tmp = disMin(A1, A2, A_ps.at(0));
                            }
                            else
                            {
                                if (disMin(A1, A2, A_ps.at(i)) < min_tmp)
                                    min_tmp = disMin(A1, A2, A_ps.at(i));
                            }
                        }
                        if (!A_ps.empty())
                            min_distance = min(min_distance, min_tmp);
                    }
                }
                else if (!silkscreen.segment.at(i).is_line && copper.at(j).segment.at(k).is_line)//Silkscreen:arc, Copper Segment:line
                {
                    min_distance = min(min(min(Point_to_Arc_MinDist(B1, silkscreen.segment.at(i)), Point_to_Arc_MinDist(B2, silkscreen.segment.at(i))), disMin(B1, B2, A1)), disMin(B1, B2, A2));

                    if (disMin(B1, B2, circle_center_A) > rA)
                    {
                        S_ps = intersection_between_CentersLine_and_Arc(silkscreen.segment.at(i), circle_center_A + VectorOp::orth_Cswap(B1 - B2));
                        for (size_t i = 0; i < S_ps.size(); i++)
                        {
                            if (i == 0)
                            {
                                min_tmp = disMin(B1, B2, S_ps.at(0));
                            }
                            else
                            {
                                if (disMin(B1, B2, S_ps.at(i)) < min_tmp)
                                    min_tmp = disMin(B1, B2, S_ps.at(i));
                            }
                        }
                        if (!S_ps.empty())
                            min_distance = min(min_distance, min_tmp);
                    }
                }
                else if (!silkscreen.segment.at(i).is_line && !copper.at(j).segment.at(k).is_line)//silkscreen: arc, copper segment arc. 
                {
                    min_distance = INFINITY;// changed on 8/16 (Warning)
                    if (circle_center_A == circle_center_B)
                    {
                        if (Concentric_Circle_On_Arc(silkscreen.segment.at(i), copper.at(j).segment.at(k))) 
                        {
                            if (rA > rB)
                                min_distance = rA - rB;
                            else
                                min_distance = rB - rA;
                        }
                    }
                    else
                    {
                        S_ps = intersection_between_CentersLine_and_Arc(silkscreen.segment.at(i), circle_center_B);
                        A_ps = intersection_between_CentersLine_and_Arc(copper.at(j).segment.at(k), circle_center_A);
                        for (size_t i = 0; i < S_ps.size(); i++)
                        {
                            for (size_t j = 0; j < A_ps.size(); j++)
                            {
                                if (i == 0 && j == 0)
                                    min_tmp = POINT::dist(S_ps.at(0), A_ps.at(0));
                                else
                                {
                                    if (POINT::dist(S_ps.at(i), A_ps.at(j)) < min_tmp)
                                        min_tmp = POINT::dist(S_ps.at(i), A_ps.at(j));
                                }
                            }
                        }
                        if (!S_ps.empty() && !A_ps.empty())
                            min_distance = min_tmp;
                    }

                    if ((A1 != A2) && (B1 != B2))
                        min_distance = min(min(min(min(Point_to_Arc_MinDist(A1, copper.at(j).segment.at(k)), Point_to_Arc_MinDist(A2, copper.at(j).segment.at(k))), Point_to_Arc_MinDist(B1, silkscreen.segment.at(i))), Point_to_Arc_MinDist(B2, silkscreen.segment.at(i))), min_distance);
                    else if ((A1 == A2) && (B1 != B2))
                        min_distance = min(min(Point_to_Arc_MinDist(B1, silkscreen.segment.at(i)), Point_to_Arc_MinDist(B2, silkscreen.segment.at(i))), min_distance);
                    else if ((A1 != A2) && (B1 == B2))
                        min_distance = min(min(Point_to_Arc_MinDist(A1, copper.at(j).segment.at(k)), Point_to_Arc_MinDist(A2, copper.at(j).segment.at(k))), min_distance);
                }

                if ((min_distance < L_copper && min_distance > L_copper - Subtraction_Tolerance)) // || (min_distance > L_copper && min_distance < L_copper + tolerance))
                    min_distance = L_copper;
                else if (min_distance < L_copper - Subtraction_Tolerance)
                {
                    pass_monitor = false;
                    cout << "Error: i(silkscreen) = " << i << ", j(copper) = " << j << ", k = " << k << " coppergap: " << L_copper << " min_distance: " << min_distance << endl;
                    cout << "Silksreen: (" << silkscreen.segment.at(i).x1 << "," << silkscreen.segment.at(i).y1 << ") -> (" << silkscreen.segment.at(i).x2 << "," << silkscreen.segment.at(i).y2 << ") is_line = " << silkscreen.segment.at(i).is_line;
                    if (silkscreen.segment.at(i).is_line == 0)
                        cout << " center: (" << silkscreen.segment.at(i).center_x << "," << silkscreen.segment.at(i).center_y << ")";
                    cout << "     Copper: (" << copper[j].segment[k].x1 << "," << copper[j].segment[k].y1 << ") -> (" << copper[j].segment[k].x2 << "," << copper[j].segment[k].y2 << ") is_line = " << copper[j].segment[k].is_line;
                    if (copper[j].segment[k].is_line == 0)
                        cout << " center: (" << copper[j].segment[k].center_x << "," << copper[j].segment[k].center_y << ")" << endl;
                    else
                        cout << endl;
                }

                if (k == 0 || min_dist_result > min_distance) // find the smallest "min_distance"
                {
                    min_dist_result = min_distance;
                }
                else
                {
                    if (min_distance < min_dist_result)
                    {
                        min_dist_result = min_distance;
                    }
                }
            }
            if (j == 0) // find the smallest "min_dist_result"
            {
                min_copper_distance = min_dist_result;
            }
            else
            {
                if (min_dist_result < min_copper_distance)
                {
                    min_copper_distance = min_dist_result;
                }
            }
        }
        if (continue_seg_count == 0) // the end of the continuous segment
        {                            // compare the distance of the begin and end.
            if (min_copper_used && (min_copper_temp < min_copper_distance))
            {
                min_distance_sum += min_copper_temp;
            }
            else
            {
                min_distance_sum += min_copper_distance;
            }
        }
        else
        {
            min_copper_temp = min_copper_distance;
            min_copper_used = true;
        }
    }
    T_copper = min_distance_sum / (double)this->continue_num.size();
    Third_Score = (1 - (T_copper - L_copper) * 10 / L_copper) * 0.25;
    // print the score of the third_quarter
    if (ShowDetail)
    {
        cout << "Third Part: ";
        if (pass_monitor)
            cout << "PASS" << endl;
        else
            cout << "FAIL" << endl;
        cout << endl;
        cout << "Score: " << Third_Score << endl
             << endl;
        cout << "T_copper: " << T_copper << endl
             << endl
             << endl;
    }
    return (Third_Score > 0.25) ? 0.25 : Third_Score;
}

// 7/10 done untest
double Scorer::fourth_quarter()
{
    float L_outline = assemblygap;
    double min_distance;
    double min_tmp;
    double shortest_min; // one silkscreen
    double continue_min; // continue silscreen
    double min_distance_sum;
    double T_outline;
    double Fourth_Score;
    Point A1, A2, B1, B2; // A,B兩線段
    double rA = 0;
    double rB = 0;
    Point circle_center_A, circle_center_B;
    int group = 0;
    int count_number = 0;
    bool brk = false;

    bool pass_monitor = true;

    min_distance_sum = 0;
    vector<Point> A_ps, S_ps;

    for (size_t i = 0; i < silkscreen.segment.size(); i++)
    {
        if (++count_number == continue_num[group])
        {
            group++;
            brk = true;
        }
        shortest_min = 0;
        A1.x = silkscreen.segment.at(i).x1;
        A1.y = silkscreen.segment.at(i).y1;
        A2.x = silkscreen.segment.at(i).x2;
        A2.y = silkscreen.segment.at(i).y2;
        circle_center_A.x = silkscreen.segment.at(i).center_x;
        circle_center_A.y = silkscreen.segment.at(i).center_y;
        rA = POINT::dist(A1, circle_center_A);
        for (size_t j = 0; j < assembly.segment.size(); j++)
        {
            B1.x = assembly.segment.at(j).x1;
            B1.y = assembly.segment.at(j).y1;
            B2.x = assembly.segment.at(j).x2;
            B2.y = assembly.segment.at(j).y2;
            circle_center_B.x = assembly.segment.at(j).center_x;
            circle_center_B.y = assembly.segment.at(j).center_y;
            rB = POINT::dist(B1, circle_center_B);
            if (silkscreen.segment.at(i).is_line && assembly.segment.at(j).is_line)//silkscreen: line, assembly segment line.
            {
                if (dir(A1, A2, B1) * dir(A1, A2, B2) <= 0 && dir(B1, B2, A1) * dir(B1, B2, A2) <= 0) //兩線段相交, 距離為0
                    min_distance = 0;
                else //如不相交, 最短距離為四個端點中到另一條線段距離的最小值
                    min_distance = min(min(min(disMin(A1, A2, B1), disMin(A1, A2, B2)), disMin(B1, B2, A1)), disMin(B1, B2, A2));
            }
            else if (silkscreen.segment.at(i).is_line && !assembly.segment.at(j).is_line)//silkscreen: line, assembly segment arc.
            {
                min_distance = min(min(min(disMin(A1, A2, B1), disMin(A1, A2, B2)), Point_to_Arc_MinDist(A1, assembly.segment.at(j))), Point_to_Arc_MinDist(A2, assembly.segment.at(j)));

                /*if (disMin(A1, A2, circle_center_B) < rB)
                {
                    if(dist(A1, circle_center_B) < rB)

                    min_distance = min(min_distance, rB - disMin(A1, A2, circle_center_B));
                    if (dist(A1, circle_center_B) < rB)
                        min_distance = min(min_distance, rB - dist(A1, circle_center_B));
                    if (dist(A2, circle_center_B) < rB)
                        min_distance = min(min_distance, rB - dist(A2, circle_center_B));
                }*/
                if (disMin(A1, A2, circle_center_B) > rB)
                {
                    //
                    A_ps = intersection_between_CentersLine_and_Arc(assembly.segment.at(j), circle_center_B + VectorOp::orth_Cswap(A1 - A2));
                    for (size_t i = 0; i < A_ps.size(); i++)
                    {
                        if (i == 0)
                        {
                            min_tmp = disMin(A1, A2, A_ps[0]);
                        }
                        else
                        {
                            if (disMin(A1, A2, A_ps[i]) < min_tmp)
                                min_tmp = disMin(A1, A2, A_ps[i]);
                        }
                    }
                    if (!A_ps.empty())
                        min_distance = min(min_distance, min_tmp);
                }
            }
            else if (!silkscreen.segment.at(i).is_line && assembly.segment.at(j).is_line)//silkscreen: arc, assembly segment line.
            {

                min_distance = min(min(min(Point_to_Arc_MinDist(B1, silkscreen.segment.at(i)), Point_to_Arc_MinDist(B2, silkscreen.segment.at(i))), disMin(B1, B2, A1)), disMin(B1, B2, A2));

                /*if (disMin(B1, B2, circle_center_A) < rA)
                {
                    min_distance = min(min_distance, rA - disMin(B1, B2, circle_center_A));
                    if (dist(B1, circle_center_A) < rA)
                        min_distance = min(min_distance, rA - dist(B1, circle_center_A));
                    if (dist(B2, circle_center_A) < rA)
                        min_distance = min(min_distance, rA - dist(B2, circle_center_A));
                }*/
                if (disMin(B1, B2, circle_center_A) > rA)
                {
                    S_ps = intersection_between_CentersLine_and_Arc(silkscreen.segment.at(i), circle_center_A + VectorOp::orth_Cswap(B1 - B2));
                    for (size_t i = 0; i < S_ps.size(); i++)
                    {
                        if (i == 0)
                        {
                            min_tmp = disMin(B1, B2, S_ps[0]);
                        }
                        else
                        {
                            if (disMin(B1, B2, S_ps[i]) < min_tmp)
                                min_tmp = disMin(B1, B2, S_ps[i]);
                        }
                    }
                    if (!S_ps.empty())
                        min_distance = min(min_distance, min_tmp);
                }
            }
            else if (!silkscreen.segment.at(i).is_line && !assembly.segment.at(j).is_line)//silkscreen: arc, assembly segment arc.
            {
                min_distance = INFINITY;
                if (circle_center_A == circle_center_B)
                { //兩個圓弧構成之連心線與圓弧焦點關係
                    if (Concentric_Circle_On_Arc(silkscreen.segment.at(i), assembly.segment.at(j)))
                    {
                        if (rA > rB)
                            min_distance = rA - rB;
                        else
                            min_distance = rB - rA;
                    }
                }
                else
                {
                    S_ps = intersection_between_CentersLine_and_Arc(silkscreen.segment.at(i), circle_center_B);
                    A_ps = intersection_between_CentersLine_and_Arc(assembly.segment.at(j), circle_center_A);
                    for (size_t i = 0; i < S_ps.size(); i++)
                    {
                        for (size_t j = 0; j < A_ps.size(); j++)
                        {
                            if (i == 0 && j == 0)
                                min_tmp = POINT::dist(S_ps[0], A_ps[0]);
                            else
                            {
                                if (POINT::dist(S_ps[i], A_ps[j]) < min_tmp)
                                    min_tmp = POINT::dist(S_ps[i], A_ps[j]);
                            }
                        }
                    }
                    if (!S_ps.empty() && !A_ps.empty())
                        min_distance = min_tmp;
                }

                if ((A1 != A2) && (B1 != B2))
                    min_distance = min(min(min(min(Point_to_Arc_MinDist(A1, assembly.segment.at(j)), Point_to_Arc_MinDist(A2, assembly.segment.at(j))), Point_to_Arc_MinDist(B1, silkscreen.segment.at(i))), Point_to_Arc_MinDist(B2, silkscreen.segment.at(i))), min_distance);
                else if ((A1 == A2) && (B1 != B2))
                    min_distance = min(min(Point_to_Arc_MinDist(B1, silkscreen.segment.at(i)), Point_to_Arc_MinDist(B2, silkscreen.segment.at(i))), min_distance);
                else if ((A1 != A2) && (B1 == B2))
                    min_distance = min(min(Point_to_Arc_MinDist(A1, assembly.segment.at(j)), Point_to_Arc_MinDist(A2, assembly.segment.at(j))), min_distance);
                // else if ((A1 == A2) && (B1 == B2))
                // min_distance = min_tmp;
            }

            if ((min_distance < L_outline && min_distance > L_outline - Subtraction_Tolerance)) // || (min_distance > L_outline && min_distance < L_outline + Subtraction_Tolerance))
                min_distance = L_outline;
            else if (min_distance < L_outline - Subtraction_Tolerance)
            {
                pass_monitor = false;
                cout << "Error: i(silkscreen) = " << i << ", j(assembly) = " << j << " assemblygap: " << L_outline << " min_distance: " << min_distance << endl;
                cout << "Silksreen: (" << silkscreen.segment.at(i).x1 << "," << silkscreen.segment.at(i).y1 << ") -> (" << silkscreen.segment.at(i).x2 << "," << silkscreen.segment.at(i).y2 << ") is_line = " << silkscreen.segment.at(i).is_line;
                if (silkscreen.segment.at(i).is_line == 0)
                    cout << " center: (" << silkscreen.segment.at(i).center_x << "," << silkscreen.segment.at(i).center_y << ")";
                cout << "     Assembly: (" << assembly.segment.at(j).x1 << "," << assembly.segment.at(j).y1 << ") -> (" << assembly.segment.at(j).x2 << "," << assembly.segment.at(j).y2 << ") is_line = " << assembly.segment.at(j).is_line;
                if (assembly.segment.at(j).is_line == 0)
                    cout << " center: (" << assembly.segment.at(j).center_x << "," << assembly.segment.at(j).center_y << ")" << endl;
                else
                    cout << endl;
            }

            if (j == 0 || shortest_min > min_distance)
                shortest_min = min_distance;
        }

        if (count_number == 1)
            continue_min = shortest_min;

        if (!brk)
            continue_min = min(continue_min, shortest_min);
        else
        {
            min_distance_sum += continue_min;
            count_number = 0;
            brk = false;
        }
    }
    T_outline = min_distance_sum / this->continue_num.size();
    Fourth_Score = (1 - (T_outline - L_outline) * 10 / L_outline) * 0.25;

    if (Fourth_Score < 0)
        Fourth_Score = 0;
    if (ShowDetail)
    {
        cout << "Fourth Part: ";
        if (pass_monitor)
            cout << "PASS" << endl;
        else
            cout << "FAIL" << endl;
        cout << endl;
        cout << "Score: " << Fourth_Score << endl
             << endl;
        cout << "T_outline: " << T_outline << endl
             << endl
             << endl;
    }
    return Fourth_Score;
}

double Scorer::Total_score(bool Print_Detail)
{
    ShowDetail = Print_Detail;
    double total_score = 0;
    if (ShowDetail)
        cout << "=========================" << endl
             << "      Score Detail       " << endl
             << "=========================" << endl
             << endl;
    double First_Score = round(first_quarter() * 10000) / 10000;
    First_Score = (First_Score > 0) ? First_Score : 0;
    double Second_Score = round(second_quarter() * 10000) / 10000;
    Second_Score = (Second_Score > 0) ? Second_Score : 0;
    double Third_Score = round(third_quarter() * 10000) / 10000;
    Third_Score = (Third_Score > 0) ? Third_Score : 0;
    double Fourth_Score = round(fourth_quarter() * 10000) / 10000;
    Fourth_Score = (Fourth_Score > 0) ? Fourth_Score : 0;
    if (ShowDetail)
        cout << "=========================" << endl
             << "   End of Score Detail   " << endl
             << "=========================" << endl
             << endl;
    cout << setw(14) << left << "First Score: " << setprecision(4) << fixed << First_Score << endl;
    cout << setw(14) << left << "Second Score: " << setprecision(4) << fixed << Second_Score << endl;
    cout << setw(14) << left << "Third Score: " << setprecision(4) << fixed << Third_Score << endl;
    cout << setw(14) << left << "Fourth Score: " << setprecision(4) << fixed << Fourth_Score << endl;
    cout << endl
         << "Total Score: ";
    total_score = First_Score + Second_Score + Third_Score + Fourth_Score;
    return total_score;
}
/*
double cross1(Point o, Point a, Point b) // move to vector op
{
    return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
}

double cross(Point v1, Point v2) // 向量外積 //same in vectorop
{
    return v1.x * v2.y - v1.y * v2.x;
}

double dot(Point v1, Point v2) // 向量內積 //same in vector op
{
    return v1.x * v2.x + v1.y * v2.y;
}

double dis2(Point A, Point B) //點A、B距離的平方 // move to Point 
{
    return (A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y);
}

double dist(Point A, Point B) //點A、B距離 // will replace by stl hypot 
{
    return sqrt((A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y));
}
*/
int Scorer::dir(Point A, Point B, Point P) //點P與線段AB位置關係 // in scorer
{
    Point v1, v2;
    v1 = B - A;
    v2 = P - A;
    if (VectorOp::cross(v1, v2) < 0)
        return -1; //逆時針
    else if (VectorOp::cross(v1, v2) > 0)
        return 1; //順時針
    else if (VectorOp::dot(v1, v2) < 0)
        return -2; //反延長線
    else if (VectorOp::dot(v1, v2) >= 0 && POINT::dis2(A, B) >= POINT::dis2(A, P))
    {
        if (POINT::dis2(A, B) < POINT::dis2(A, P))
            return 2; //延長線
        return 0;     //在線上
    }
    return -100;
}

double Scorer::disMin(const Point A, const Point B, const Point P) //點P到線段AB的最短距離
{
    double r = ((P.x - A.x) * (B.x - A.x) + (P.y - A.y) * (B.y - A.y)) / POINT::dis2(A, B);
    if (r <= 0)
        return POINT::dist(A, P);
    else if (r >= 1)
        return POINT::dist(B, P);
    else
    {
        double AC = r * sqrt(POINT::dis2(A, B));
        return sqrt(POINT::dis2(A, P) - AC * AC);
    }
}

//retain in the Socorer
double Scorer::Point_to_Arc_MinDist(Point pp, Segment Arc) //點到圓弧之最短距離
{
    Point p1, p2, pc, ex_p;
    p1.x = Arc.x1;
    p1.y = Arc.y1;
    p2.x = Arc.x2;
    p2.y = Arc.y2;
    pc.x = Arc.center_x;
    pc.y = Arc.center_y;
    double radius = POINT::dist(p1, pc);

    Point v1;
    v1 = pp - pc;
    ex_p = pc + radius / POINT::dist(pp, pc) * v1;

    if (On_Arc(Arc, ex_p) || p1 == p2)
    {
        if (POINT::dist(pp, pc) > radius)
        {
            return POINT::dist(pp, pc) - radius;
        }

        else
        {
            return radius - POINT::dist(pp, pc);
        }
    }
    else
    {
        return min(POINT::dist(pp, p1), POINT::dist(pp, p2));
    }
}

vector<Point> Scorer::intersection_between_CentersLine_and_Arc(Segment Arc, Point Center) // the other arc's center
{                                                                                 // 圓心線對Arc的交點
    Point centerpoint, A;
    Point v1, v2;
    Point tmp;
    vector<Point> intersect;
    double radius;
    double theta;

    A.x = Arc.x1;
    A.y = Arc.y1;
    centerpoint.x = Arc.center_x;
    centerpoint.y = Arc.center_y;
    radius = POINT::dist(A, centerpoint);

    theta = atan2(centerpoint.y - Center.y, centerpoint.x - Center.x);
    v1 = Center - centerpoint;
    tmp.x = centerpoint.x + radius * cos(theta);
    tmp.y = centerpoint.y + radius * sin(theta);

    if (On_Arc(Arc, tmp))
    {
        intersect.push_back(tmp);
    }

    v2 = centerpoint - tmp;
    tmp = tmp + 2 * v2;

    if (On_Arc(Arc, tmp))
    {
        intersect.push_back(tmp);
    }
    return intersect;
}

Point Scorer::find_arbitary_point_on_arc(Segment Arc) //找出Arc兩端外圓上一點
{

    Point middlepoint; // A,B中點
    Point centerpoint, A, B;
    Point v1, v2;
    double radius;

    A.x = Arc.x1;
    A.y = Arc.y1;
    B.x = Arc.x2;
    B.y = Arc.y2;

    middlepoint = (A + B) / 2;
    centerpoint.x = Arc.center_x;
    centerpoint.y = Arc.center_y;
    radius = POINT::dist(A, centerpoint);
    v1 = centerpoint - middlepoint;
    v2 = B - A;
    if ((A + B) / 2 == centerpoint)
    {
        if (!Arc.is_CCW )
            return middlepoint + VectorOp::orth_Cswap(v2 / 2);
        else
            return middlepoint + VectorOp::orth_CCswap(v2 / 2);
    }
    else if ((VectorOp::cross(v2, v1) > 0 && Arc.is_CCW) || (VectorOp::cross(v2, v1) < 0 && !Arc.is_CCW))
        return middlepoint + (radius - POINT::dist(middlepoint, centerpoint)) / POINT::dist(middlepoint, centerpoint) * (-1 * v1);
    else
        return middlepoint + (POINT::dist(middlepoint, centerpoint) + radius) / POINT::dist(middlepoint, centerpoint) * v1;
}

bool Scorer::On_Arc(Segment Arc, Point p) //判斷點P是否在Arc上
{
    Segment AB, BC, OP;
    Point ar_p;
    ar_p = find_arbitary_point_on_arc(Arc);
    AB.x1 = Arc.x1;
    AB.y1 = Arc.y1;
    AB.x2 = ar_p.x;
    AB.y2 = ar_p.y;
    BC.x1 = ar_p.x;
    BC.y1 = ar_p.y;
    BC.x2 = Arc.x2;
    BC.y2 = Arc.y2;

    OP.x1 = Arc.center_x;
    OP.y1 = Arc.center_y;
    OP.x2 = p.x;
    OP.y2 = p.y;

    return (graph_op.Line_intersect(AB, OP) || graph_op.Line_intersect(BC, OP))? true : false;
}

bool Scorer::Concentric_Circle_On_Arc(Segment Arc1, Segment Arc2) //同心圓對兩Arc端點射線，在Arc是否有交點
{
    Point A1, A2, B1, B2;
    Point Center;
    double radius_A, radius_B;
    Point v1, v2, v3, v4;

    A1.x = Arc1.x1;
    A1.y = Arc1.y1;
    A2.x = Arc1.x2;
    A2.y = Arc1.y2;
    B1.x = Arc2.x1;
    B1.y = Arc2.y1;
    B2.x = Arc2.x2;
    B2.y = Arc2.y2;
    Center.x = Arc1.center_x;
    Center.y = Arc1.center_y;

    radius_A = POINT::dist(A1, Center);
    radius_B = POINT::dist(B1, Center);

    v1 = A1 - Center;
    v2 = A2 - Center;
    v3 = B1 - Center;
    v4 = B2 - Center;

    if (On_Arc(Arc1, Center + radius_A / radius_B * v3) == 1 || On_Arc(Arc1, Center + radius_A / radius_B * v4) == 1 || On_Arc(Arc2, Center + radius_B / radius_A * v1) == 1 || On_Arc(Arc2, Center + radius_B / radius_A * v2) == 1)
        return 1;
    else
        return 0;
}

// vector<Point> Orth_Point_On_Arc(Segment Arc, Segment Line)
// {
//     Point centerpoint, A, B, ex_p;
//     Point v1, v2;
//     double theta;
//     double radius;

//     centerpoint.x = Arc.center_x;
//     centerpoint.y = Arc.center_y;
//     A.x = Line.x1;
//     A.y = Line.y1;
//     B.x = Line.x2;
//     B.y = Line.y2;
//     radius = dist(A, centerpoint);

//     v1 = A - B;
//     v2 = orth_Cswap(v1);
//     theta = atan2(v2.x, v2.y);
//     ex_p = centerpoint + radius * cos(theta);
//     ex_p = centerpoint + radius * sin(theta);
//     if(dist(ex_p, centerpoint) > disMin(centerpoint, Line))

// }
/*
bool Line_intersect(Segment S1, Segment S2)//找兩Segment交點 //moved to GRAPH
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
    double c1 = cross1(a1, a2, b1);
    double c2 = cross1(a1, a2, b2);
    double c3 = cross1(b1, b2, a1);
    double c4 = cross1(b1, b2, a2);

    // 端點不共線
    if (c1 * c2 <= 0 && c3 * c4 <= 0)
        return true;
    // 端點共線
    
    //if (c1 == 0 && Line_intersect(a1, a2, b1)) return true;
    //if (c2 == 0 && Line_intersect(a1, a2, b2)) return true;
    //if (c3 == 0 && Line_intersect(b1, b2, a1)) return true;
    //if (c4 == 0 && Line_intersect(b1, b2, a2)) return true;
    
    return false;
}
*/

// enable "main" to modify the private data member, with cascading
Scorer &Scorer::setAssembly(const Graph Assembly)
{
    this->assembly = Assembly;
    return *this;
}
Scorer &Scorer::setCopper(const vector<Graph> copper)
{
    this->copper = copper;
    return *this;
}
Scorer &Scorer::setSilkscreen(const Graph silkscreen)
{
    this->silkscreen = silkscreen;
    return *this;
}


Point operator-(Point a, Point b)
{
    Point v;
    v.x = a.x - b.x;
    v.y = a.y - b.y;
    return v;
}

Point operator+(Point a, Point b)
{
    Point v;
    v.x = a.x + b.x;
    v.y = a.y + b.y;
    return v;
}

Point operator*(double c, Point a) //@overload,伸縮
{
    Point v;
    v.x = c * a.x;
    v.y = c * a.y;
    return v;
}

Point operator/(Point a, double c) //@overload,伸縮
{
    Point v;
    v.x = a.x / c;
    v.y = a.y / c;
    return v;
}

bool operator==(Point a, Point b)
{
    if (a.x == b.x && a.y == b.y)
        return 1;
    else
        return 0;
}

bool operator!=(Point a, Point b)
{
    if (a.x == b.x && a.y == b.y)
        return 0;
    else
        return 1;
}
/*
Point orth_Cswap(Point a) //垂直順時鐘(向量) //moved to VectorOp
    Point v;
    v.x = a.y * (-1);
    v.y = a.x;
    return v;
}

Point orth_CCswap(Point a) //垂直逆時鍾(向量) // moved to VectorOp
{
    Point v;
    v.x = a.y;
    v.y = a.x * (-1);
    return v;
}
*/
