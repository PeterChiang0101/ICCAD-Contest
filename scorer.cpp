// functions of scorer.h

#include <bits/stdc++.h>
#include "scorer.h"
#include "inputoutput.h"

using namespace std;

#define PI 3.14159265358979323846
#define eps 1e-8
#define INPUT_PATH "./TestingCase/test_B.txt"
#define OUTPUT_PATH "./TestingCase/test_B_Ans.txt"

//=================function declarations==========================
double Arc_Degree(const Segment &); // calculate the Segment theta
double cross(Point, Point);
double dot(Point, Point);
double dis2(Point, Point);
double dist(Point, Point);
int dir(Point, Point, Point);
double disMin(Point, Point, Point);
Point operator-(Point, Point);
Point operator+(Point, Point);
Point operator*(double, Point);
Point operator/(Point, double);
bool operator==(Point, Point);
bool operator!=(Point, Point);
Point orth_Cswap(Point);
Point orth_CCswap(Point);
double Point_to_Arc_MinDist(Point, Segment);
vector<Point> intersection_between_CentersLine_and_Arc(Segment, Point);
bool Line_intersect(Segment, Segment);
bool On_Arc(Segment, Point);
bool Concentric_Circle_On_Arc(Segment, Segment);

Scorer::Scorer()
{ // the default constructor, use the defined Q_FILE and A_FILE path
    this->Q_file.open(INPUT_PATH, ios::in);
    this->A_file.open(OUTPUT_PATH, ios::in);
    this->open_file();
}

Scorer::Scorer(const char *Ques_File, const char *Ans_File)
{ // modify the Q_FILE and A_FILE path
    this->Q_file.open(Ques_File, ios::in);
    this->A_file.open(Ans_File, ios::in);
    this->open_file();
}

void Scorer::open_file()
{
    Input_Output A;
    // Q_file.open(INPUT_PATH, ios::in);
    // A_file.open(OUTPUT_PATH, ios::in);
    string assemblygap_str, coppergap_str, silkscreenlen_str;
    Q_file >> assemblygap_str >> coppergap_str >> silkscreenlen_str;
    assemblygap = A.File_to_Parameter(assemblygap_str);
    coppergap = A.File_to_Parameter(coppergap_str);
    silkscreenlen = A.File_to_Parameter(silkscreenlen_str);
    assembly = A.Read_Assembly(Q_file);
    copper = A.Read_Copper(Q_file);
    silkscreen = Read_Silkscreen(A_file);
    Assembly_push_out = A.Assembly_Buffer(this->assembly, this->coppergap, this->assemblygap);
}

// untest 2022/7/7
double Scorer::first_quarter() // const vector<Segment> Assembly, const vector<Segment> silkscreen)
{
    Input_Output A1;
    float Rectangular_area = 0;       // 絲印標示之座標極限值所構成之矩形面積
    float X_max, Y_max, X_min, Y_min; //絲印座標極限值

    float Y_area = 0; // 零件外觀向外等比拓展Y之面積範圍
    // vector<Segment> Assembly_push_out; 7/10, move to class private area.

    float Answer_1;

    /* calculate Rectangular_area */
    X_max = this->silkscreen[0].x1;
    X_min = this->silkscreen[0].x2;
    Y_max = this->silkscreen[0].y1;
    Y_min = this->silkscreen[0].y2;

    for (size_t i = 0; i < this->silkscreen.size(); i++)
    {
        if (this->silkscreen[i].x2 > X_max)
            X_max = this->silkscreen[i].x2;
        if (this->silkscreen[i].x1 > X_max)
            X_max = this->silkscreen[i].x1;
        if (this->silkscreen[i].x2 < X_min)
            X_min = this->silkscreen[i].x2;
        if (this->silkscreen[i].x1 < X_min)
            X_min = this->silkscreen[i].x1;

        if (this->silkscreen[i].y2 > Y_max)
            Y_max = this->silkscreen[i].y2;
        if (this->silkscreen[i].y1 > Y_max)
            Y_max = this->silkscreen[i].y1;
        if (this->silkscreen[i].y2 < Y_min)
            Y_min = this->silkscreen[i].y2;
        if (this->silkscreen[i].y1 < Y_min)
            Y_min = this->silkscreen[i].y1;
    }
    Rectangular_area = abs((X_max - X_min) * (Y_max - Y_min));

    /* calculate Y_area*/
    vector<Point> Assembly_push_out_points;
    vector<vector<Point>> Arc_Dots;
    // this->Assembly_push_out = A.Assembly_Buffer(Assembly, coppergap, assemblygap);
    Assembly_push_out_points = A1.Line_to_Point(Assembly_push_out);
    Arc_Dots = A1.Arc_Optimization(Assembly_push_out);

    float total_area = 0;
    size_t i;
    size_t j = i - 1;
    for (i = 0; i < Assembly_push_out.size(); i++)
    {
        if (i == 0)
            j = Assembly_push_out.size() - 1;
        else
            j = i - 1;
        total_area += (Assembly_push_out[j].x1 + Assembly_push_out[i].x1) * (Assembly_push_out[j].y1 - Assembly_push_out[i].y1);
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
    for (i = 0; i < Assembly_push_out.size(); i++)
    {
        if (!Assembly_push_out.at(i).is_line)
        {
            center_of_circle.x = Assembly_push_out.at(i).center_x;
            center_of_circle.y = Assembly_push_out.at(i).center_y;
            point_1.x = Assembly_push_out.at(i).x1;
            point_1.y = Assembly_push_out.at(i).y1;
            point_2.x = Assembly_push_out.at(i).x2;
            point_2.y = Assembly_push_out.at(i).y2;

            radius = sqrt(dis2(center_of_circle, point_1));
            c = sqrt(dis2(point_1, point_2));
            s = (radius + radius + c) / 2;
            testing_variable = (2 * (radius * radius) - c * c) / (2 * radius * radius);
            theta = acos(testing_variable); // Law of cosines

            cut_area = radius * radius * theta / 2 - sqrt(s * (s - radius) * (s - radius) * (s - c)); // last part is heron formula

            outside = !A1.point_in_polygon(center_of_circle, Assembly_push_out_points, Arc_Dots);
            if (outside)
                total_area -= cut_area;
            else
                total_area += cut_area;
        }
    }
    total_area /= 2; //修正項
    Answer_1 = (2 - Rectangular_area / (total_area)) * 0.25;
    cout << "First Score:" << Answer_1 << endl
         << endl;
    cout << "Rectangular_area:" << Rectangular_area << endl;
    cout << "total_area:" << total_area << endl
         << endl
         << endl;

    return (Answer_1 > 0.25) ? 0.25 : Answer_1; //大於0.25只算0.25
}

// not finish verification, done on 2022/7/9
double Scorer::second_quarter() // const vector<Segment>Assembly, const vector<Segment> silkscreen)
{
    double part_1{0}, part_2{0}, Second_Score{0}, total_perimeter{0}, total_silkscreen{0};
    int assembly_line{0}, assembly_arc{0}, silk_line{0}, silk_arc{0};
    double number_diff{0};
    // First Part
    // notice!! the buffer of the assembly, not sure it is as same as description.
    // calculate the Perimeter of assembly
    double length{0}, length_x{0}, length_y{0}, radius{0};
    for (size_t i = 0; i < Assembly_push_out.size(); i++)
    {
        length = 0;
        if (Assembly_push_out[i].is_line) // line
        {
            length_x = Assembly_push_out[i].x2 - Assembly_push_out[i].x1;
            length_y = Assembly_push_out[i].y2 - Assembly_push_out[i].y1;
            length = hypot(length_x, length_y);
        }
        else // arc
        {
            length_x = Assembly_push_out[i].center_x - Assembly_push_out[i].x1;
            length_y = Assembly_push_out[i].center_y - Assembly_push_out[i].y1;
            radius = hypot(length_x, length_y);
            length = radius * Arc_Degree(Assembly_push_out[i]);
        }
        total_perimeter += length;
    }
    // read the Answer Silkscreen and count the number of Line and Arc.
    for (size_t i = 0; i < this->silkscreen.size(); i++)
    {
        length = 0;
        if (this->silkscreen[i].is_line)
        {
            length_x = this->silkscreen[i].x2 - this->silkscreen[i].x1;
            length_y = this->silkscreen[i].y2 - this->silkscreen[i].y1;
            length = hypot(length_x, length_y);
            silk_line += 1;
        }
        else
        {
            length_x = this->silkscreen[i].center_x - this->silkscreen[i].x1;
            length_y = this->silkscreen[i].center_y - this->silkscreen[i].y1;
            radius = hypot(length_x, length_y);
            length = radius * Arc_Degree(this->silkscreen[i]);
            silk_arc += 1;
        }
        total_silkscreen += length;
    }
    // Second_part
    // count the number of assembly Line and Arc
    for (size_t i = 0; i < this->assembly.size(); i++)
    {
        if (this->assembly[i].is_line)
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
    part_2 = (1 - (number_diff / (double)(this->assembly.size() + this->copper.size())) > 1) ? 1 : (1 - (number_diff / (double)(this->assembly.size() + this->copper.size())));
    Second_Score = part_1 * 0.15 + part_2 * 0.10;
    // print the result of second quarter
    cout << "Second Score: " << Second_Score << endl
         << endl;
    cout << "Part_1 score: " << part_1 << endl
         << "Part_2 Score: " << part_2 << endl
         << endl
         << endl;

    return Second_Score;
}

double Arc_Degree(const Segment &S1)
{
    double degree{0};
    if (!S1.is_line)
    {
        degree = abs(S1.theta_1 - S1.theta_2);
        if (S1.theta_1 > 0 && S1.theta_2 < 0 && S1.direction)
        {
            degree = 2 * PI - degree;
        }
        else
        {
            if (S1.theta_1 < 0 && S1.theta_2 > 0 && !S1.direction)
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

    min_distance_sum = 0;

    for (size_t i = 0; i < this->silkscreen.size(); i++)
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

        A1.x = this->silkscreen.at(i).x1;
        A1.y = this->silkscreen.at(i).y1;
        A2.x = this->silkscreen.at(i).x2;
        A2.y = this->silkscreen.at(i).y2;
        rA = dist(A1, circle_center_A);

        for (size_t j = 0; j < this->copper.size(); j++)
        {
            for (size_t k = 0; k < this->copper.at(j).size(); k++)
            {
                B1.x = this->copper.at(j).at(k).x1;
                B1.y = this->copper.at(j).at(k).y1;
                B2.x = this->copper.at(j).at(k).x2;
                B2.y = this->copper.at(j).at(k).y2;
                circle_center_B.x = copper.at(j).at(k).center_x;
                circle_center_B.y = copper.at(j).at(k).center_y;
                rB = dist(B1, circle_center_B);
                if (dir(A1, A2, B1) * dir(A1, A2, B2) <= 0 && dir(B1, B2, A1) * dir(B1, B2, A2) <= 0) //兩線段相交, 距離為0
                    min_distance = 0;

                if (this->silkscreen.at(i).is_line == 1 && this->copper.at(j).at(k).is_line == 1)
                {
                    if (dir(A1, A2, B1) * dir(A1, A2, B2) <= 0 && dir(B1, B2, A1) * dir(B1, B2, A2) <= 0) //兩線段相交, 距離為0
                        min_distance = 0;
                    else //如不相交, 最短距離為四個端點中到另一條線段距離的最小值
                        min_distance = min(min(min(disMin(A1, A2, B1), disMin(A1, A2, B2)), disMin(B1, B2, A1)), disMin(B1, B2, A2));
                }
                else if (this->silkscreen[i].is_line == 1 && this->copper.at(j).at(k).is_line == 0)
                {
                    min_distance = min(min(min(disMin(A1, A2, B1), disMin(A1, A2, B2)), Point_to_Arc_MinDist(A1, copper.at(j).at(k))), Point_to_Arc_MinDist(A2, copper.at(j).at(k)));

                    if (disMin(A1, A2, circle_center_B) > rB)
                    {
                        A_ps = intersection_between_CentersLine_and_Arc(this->copper.at(j).at(k), circle_center_B + orth_Cswap(A1 - A2));
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
                else if (this->silkscreen.at(i).is_line == 0 && this->copper.at(j).at(k).is_line == 1)
                {
                    min_distance = min(min(min(Point_to_Arc_MinDist(B1, silkscreen.at(i)), Point_to_Arc_MinDist(B2, silkscreen.at(i))), disMin(B1, B2, A1)), disMin(B1, B2, A2));

                if (disMin(B1, B2, circle_center_A) > rA)
                {
                    S_ps = intersection_between_CentersLine_and_Arc(this->silkscreen.at(i), circle_center_A + orth_Cswap(B1 - B2));
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
                else if (this->silkscreen.at(i).is_line == 0 && this->copper.at(j).at(k).is_line == 0)
                {
                    if (circle_center_A == circle_center_B)
                    {
                        if (Concentric_Circle_On_Arc(silkscreen[i], copper.at(j).at(k)) == 1)
                        {
                            if (rA > rB)
                                min_distance = rA - rB;
                            else
                                min_distance = rB - rA;
                        }
                    }
                    else
                    {
                        S_ps = intersection_between_CentersLine_and_Arc(this->silkscreen.at(i), circle_center_B);
                        A_ps = intersection_between_CentersLine_and_Arc(copper.at(j).at(k), circle_center_A);
                        for (size_t i = 0; i < S_ps.size(); i++)
                        {
                            for (size_t j = 0; j < A_ps.size(); j++)
                            {
                                if (i == 0 && j == 0)
                                    min_tmp = dist(S_ps.at(0), A_ps.at(0));
                                else
                                {
                                    if (dist(S_ps.at(i), A_ps.at(j)) < min_tmp)
                                        min_tmp = dist(S_ps.at(i), A_ps.at(j));
                                }
                            }
                        }
                        if (!S_ps.empty() && !A_ps.empty())
                            min_distance = min_tmp;
                    }

                    if ((A1 != A2) && (B1 != B2))
                        min_distance = min(min(min(min(Point_to_Arc_MinDist(A1, copper.at(j).at(k)), Point_to_Arc_MinDist(A2, copper.at(j).at(k))), Point_to_Arc_MinDist(B1, silkscreen[i])), Point_to_Arc_MinDist(B2, silkscreen[i])), min_distance);
                    else if ((A1 == A2) && (B1 != B2))
                        min_distance = min(min(Point_to_Arc_MinDist(B1, silkscreen[i]), Point_to_Arc_MinDist(B2, silkscreen[i])), min_distance);
                    else if ((A1 != A2) && (B1 == B2))
                        min_distance = min(min(Point_to_Arc_MinDist(A1, copper.at(j).at(k)), Point_to_Arc_MinDist(A2, copper.at(j).at(k))), min_distance);
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
    cout << "Third Score: " << Third_Score << endl
         << endl;
    cout << "T_copper: " << T_copper << endl
         << endl
         << endl;
    return (Third_Score > 0.25) ? 0.25 : Third_Score;
}

// 7/10 done untest
double Scorer::fourth_quarter() // const vector<Segment> assembly, const vector<Segment> silkscreen
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

    min_distance_sum = 0;
    vector<Point> A_ps, S_ps;

    for (size_t i = 0; i < this->silkscreen.size(); i++)
    {
        if (++count_number == continue_num[group])
        {
            group++;
            brk = true;
        }
        shortest_min = 0;
        A1.x = this->silkscreen[i].x1;
        A1.y = this->silkscreen[i].y1;
        A2.x = this->silkscreen[i].x2;
        A2.y = this->silkscreen[i].y2;
        circle_center_A.x = this->silkscreen[i].center_x;
        circle_center_A.y = this->silkscreen[i].center_y;
        rA = dist(A1, circle_center_A);
        for (size_t j = 0; j < assembly.size(); j++)
        {
            B1.x = assembly[j].x1;
            B1.y = assembly[j].y1;
            B2.x = assembly[j].x2;
            B2.y = assembly[j].y2;
            circle_center_B.x = assembly[j].center_x;
            circle_center_B.y = assembly[j].center_y;
            rB = dist(B1, circle_center_B);
            if (this->silkscreen[i].is_line == 1 && assembly[j].is_line == 1)
            {
                if (dir(A1, A2, B1) * dir(A1, A2, B2) <= 0 && dir(B1, B2, A1) * dir(B1, B2, A2) <= 0) //兩線段相交, 距離為0
                    min_distance = 0;
                else //如不相交, 最短距離為四個端點中到另一條線段距離的最小值
                    min_distance = min(min(min(disMin(A1, A2, B1), disMin(A1, A2, B2)), disMin(B1, B2, A1)), disMin(B1, B2, A2));
            }
            else if (this->silkscreen[i].is_line == 1 && assembly[j].is_line == 0)
            {
                min_distance = min(min(min(disMin(A1, A2, B1), disMin(A1, A2, B2)), Point_to_Arc_MinDist(A1, assembly[j])), Point_to_Arc_MinDist(A2, assembly[j]));

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
                    A_ps = intersection_between_CentersLine_and_Arc(this->assembly[j], circle_center_B + orth_Cswap(A1 - A2));
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
            else if (this->silkscreen[i].is_line == 0 && assembly[j].is_line == 1)
            {

                min_distance = min(min(min(Point_to_Arc_MinDist(B1, silkscreen[i]), Point_to_Arc_MinDist(B2, silkscreen[i])), disMin(B1, B2, A1)), disMin(B1, B2, A2));

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
                    S_ps = intersection_between_CentersLine_and_Arc(this->silkscreen[i], circle_center_A + orth_Cswap(B1 - B2));
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
            else if (this->silkscreen[i].is_line == 0 && assembly[j].is_line == 0)
            {
                if (circle_center_A == circle_center_B)
                {
                    if (Concentric_Circle_On_Arc(silkscreen[i], assembly[j]) == 1)
                    {
                        if (rA > rB)
                            min_distance = rA - rB;
                        else
                            min_distance = rB - rA;
                    }
                }
                else
                {
                    S_ps = intersection_between_CentersLine_and_Arc(this->silkscreen[i], circle_center_B);
                    A_ps = intersection_between_CentersLine_and_Arc(assembly[j], circle_center_A);
                    for (size_t i = 0; i < S_ps.size(); i++)
                    {
                        for (size_t j = 0; j < A_ps.size(); j++)
                        {
                            if (i == 0 && j == 0)
                                min_tmp = dist(S_ps[0], A_ps[0]);
                            else
                            {
                                if (dist(S_ps[i], A_ps[j]) < min_tmp)
                                    min_tmp = dist(S_ps[i], A_ps[j]);
                            }
                        }
                    }
                    if (!S_ps.empty() && !A_ps.empty())
                        min_distance = min_tmp;
                }

                if ((A1 != A2) && (B1 != B2))
                    min_distance = min(min(min(min(Point_to_Arc_MinDist(A1, assembly[j]), Point_to_Arc_MinDist(A2, assembly[j])), Point_to_Arc_MinDist(B1, silkscreen[i])), Point_to_Arc_MinDist(B2, silkscreen[i])), min_distance);
                else if ((A1 == A2) && (B1 != B2))
                    min_distance = min(min(Point_to_Arc_MinDist(B1, silkscreen[i]), Point_to_Arc_MinDist(B2, silkscreen[i])), min_distance);
                else if ((A1 != A2) && (B1 == B2))
                    min_distance = min(min(Point_to_Arc_MinDist(A1, assembly[j]), Point_to_Arc_MinDist(A2, assembly[j])), min_distance);
                // else if ((A1 == A2) && (B1 == B2))
                // min_distance = min_tmp;
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

    cout << "Fourth Score: " << Fourth_Score << endl
         << endl;
    cout << "T_outline: " << T_outline << endl
         << endl;
    return Fourth_Score;
}

double Scorer::Total_score()
{
    double total_score = 0;
    cout << "Score Detail:" << endl
         << endl;
    double First_Score = round(first_quarter() * 10000) / 10000;
    double Second_Score = round(second_quarter() * 10000) / 10000;
    double Third_Score = round(third_quarter() * 10000) / 10000;
    double Fourth_Score = round(fourth_quarter() * 10000) / 10000;
    cout << "First Score: " << setprecision(4) << fixed << First_Score << endl
         << endl;
    cout << "Second Score: " << setprecision(4) << fixed << Second_Score << endl
         << endl;
    cout << "Third Score: " << setprecision(4) << fixed << Third_Score << endl

         << endl;
    cout << "Fourth Score: " << setprecision(4) << fixed << Fourth_Score << endl
         << endl;

    cout << "End of Score Detail" << endl
         << endl;
    cout << "Total Score: ";
    total_score = First_Score + Second_Score + Third_Score + Fourth_Score;
    return total_score;
}

vector<Segment> Scorer::Read_Silkscreen(fstream &Input_File)
{
    Input_Output A;
    vector<Segment> Silkscreen;
    Segment part;
    string line;
    int continue_count;

    while (getline(Input_File, line))
    {
        if (line == "silkscreen")
        {
            continue_count = 0;
            continue_num.push_back(continue_count);
            continue;
        }
        else if (line != "")
        {
            part = A.String_to_Line(line);
            continue_num.back() = (++continue_count);
            Silkscreen.push_back(part);
        }
    }
    return Silkscreen;
}

double cross1(Point o, Point a, Point b)
{
    return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
}

double cross(Point v1, Point v2) // 向量外積
{
    return v1.x * v2.y - v1.y * v2.x;
}

double dot(Point v1, Point v2) // 向量內積
{
    return v1.x * v2.x + v1.y * v2.y;
}

double dis2(Point A, Point B) //點A、B距離的平方
{
    return (A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y);
}

double dist(Point A, Point B) //點A、B距離
{
    return sqrt((A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y));
}

int dir(Point A, Point B, Point P) //點P與線段AB位置關係
{
    Point v1, v2;
    v1 = B - A;
    v2 = P - A;
    if (cross(v1, v2) < 0)
        return -1; //逆時針
    else if (cross(v1, v2) > 0)
        return 1; //順時針
    else if (dot(v1, v2) < 0)
        return -2; //反延長線
    else if (dot(v1, v2) >= 0 && dis2(A, B) >= dis2(A, P))
    {
        if (dis2(A, B) < dis2(A, P))
            return 2; //延長線
        return 0;     //在線上
    }
    return -100;
}

double disMin(Point A, Point B, Point P) //點P到線段AB的最短距離
{
    double r = ((P.x - A.x) * (B.x - A.x) + (P.y - A.y) * (B.y - A.y)) / dis2(A, B);
    if (r <= 0)
        return sqrt(dis2(A, P));
    else if (r >= 1)
        return sqrt(dis2(B, P));
    else
    {
        double AC = r * sqrt(dis2(A, B));
        return sqrt(dis2(A, P) - AC * AC);
    }
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

Point operator*(double c, Point a)
{
    Point v;
    v.x = c * a.x;
    v.y = c * a.y;
    return v;
}

Point operator/(Point a, double c)
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

Point orth_Cswap(Point a)
{
    Point v;
    v.x = a.y * (-1);
    v.y = a.x;
    return v;
}

Point orth_CCswap(Point a)
{
    Point v;
    v.x = a.y;
    v.y = a.x * (-1);
    return v;
}

/*double Point_to_Arc_MinDist(Point pp, Segment Arc)
{
    Point p1, p2, pc;
    p1.x = Arc.x1;
    p1.y = Arc.y1;
    p2.x = Arc.x2;
    p2.y = Arc.y2;
    pc.x = Arc.center_x;
    pc.y = Arc.center_y;

    Point v1, v2, v3;
    v1 = p1 - pc;
    v2 = p2 - pc;
    v3 = pp - pc;

    if (Arc.direction == 0) //如果是順時針，把p1和p2點互換
    {
        Point t = p1;
        p1 = p2;
        p2 = t;
    }

    double cosA = dot(v1, v2) / (dist(p1, pc) * dist(p2, pc)); // Arc cos(v1與v2的夾角)
    if (fabs(cosA) > 1)                                        // if fabs(cosA)>1, then acos(cosA) error
    {
        if (cosA < 0)
            cosA += eps;
        else
            cosA -= eps;
    }
    double maxd = acos(cosA); // v1與v2的夾角
    if (cross(v1, v2) < 0 && fabs(cross(v1, v2)) > eps)
        maxd = 2 * PI - maxd;
    double cosB = dot(v1, v3) / (dist(p1, pc) * dist(pp, pc));
    if (fabs(cosB) > 1)
    {
        if (cosB < 0)
            cosB += eps;
        else
            cosB -= eps;
    }
    double degree = acos(cosB); // v1與v3的夾角

    if (cross(v1, v3) < 0 && fabs(cross(v1, v3)) > eps)
        degree = 2 * PI - degree;
    if (degree < maxd)
        return fabs(dist(pp, pc) - dist(p1, pc));
    else
        return min(dist(pp, p1), dist(pp, p2));
}*/
double Point_to_Arc_MinDist(Point pp, Segment Arc)
{
    Point p1, p2, pc, ex_p;
    p1.x = Arc.x1;
    p1.y = Arc.y1;
    p2.x = Arc.x2;
    p2.y = Arc.y2;
    pc.x = Arc.center_x;
    pc.y = Arc.center_y;
    double radius;
    radius = dist(p1, pc);

    Point v1;
    v1 = pp - pc;
    ex_p = pc + radius / dist(pp, pc) * v1;

    if (On_Arc(Arc, ex_p) || p1 == p2)
    {
        if (dist(pp, pc) > radius)
            return dist(pp, pc) - radius;
        else
            return radius - dist(pp, pc);
    }
    else
        return min(dist(pp, p1), dist(pp, p2));
}

vector<Point> intersection_between_CentersLine_and_Arc(Segment Arc, Point Center) // the other arc's center
{
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
    radius = dist(A, centerpoint);

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

Point find_arbitary_point_on_arc(Segment Arc)
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
    radius = dist(A, centerpoint);
    v1 = centerpoint - middlepoint;
    v2 = B - A;
    if ((A + B) / 2 == centerpoint)
    {
        if (Arc.direction == 0)
            return middlepoint + orth_Cswap(v2 / 2);
        else
            return middlepoint + orth_CCswap(v2 / 2);
    }
    else if ((cross(v2, v1) > 0 && Arc.direction == 1) || (cross(v2, v1) < 0 && Arc.direction == 0))
        return middlepoint + (radius - dist(middlepoint, centerpoint)) / dist(middlepoint, centerpoint) * (-1 * v1);
    else
        return middlepoint + (dist(middlepoint, centerpoint) + radius) / dist(middlepoint, centerpoint) * v1;
}

bool On_Arc(Segment Arc, Point p)
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

    if (Line_intersect(AB, OP) || Line_intersect(BC, OP))
        return true;
    else
        return false;
}

bool Concentric_Circle_On_Arc(Segment Arc1, Segment Arc2)
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

    radius_A = dist(A1, Center);
    radius_B = dist(B1, Center);

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

bool Line_intersect(Segment S1, Segment S2)
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
    /*if (c1 == 0 && Line_intersect(a1, a2, b1)) return true;
    if (c2 == 0 && Line_intersect(a1, a2, b2)) return true;
    if (c3 == 0 && Line_intersect(b1, b2, a1)) return true;
    if (c4 == 0 && Line_intersect(b1, b2, a2)) return true;*/
    return false;
}

// enable "main" to modify the private data member, with cascading
Scorer &Scorer::setAssembly(const vector<Segment> Assembly)
{
    this->assembly = Assembly;
    return *this;
}
Scorer &Scorer::setCopper(const vector<vector<Segment>> copper)
{
    this->copper = copper;
    return *this;
}
Scorer &Scorer::setSilkscreen(const vector<Segment> silkscreen)
{
    this->silkscreen = silkscreen;
    return *this;
}
