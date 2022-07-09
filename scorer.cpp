// functions of scorer.h

#include <bits/stdc++.h>
#include "scorer.h"
#include "inputoutput.h"

using namespace std;

#define PI 3.14159265358979323846
#define eps 1e-8
#define INPUT_PATH "./TestingCase/test_B.txt"
#define OUTPUT_PATH "./TestingCase/test_B_Ans.txt"

double cross(Point, Point);
double dot(Point, Point);
double dis2(Point, Point);
double dist(Point, Point);
int dir(Point, Point, Point);
double disMin(Point, Point, Point);
Point operator - (Point, Point);
Point operator + (Point, Point);
Point operator * (const int, Point);
Point operator / (Point, const int);
double Point_to_Arc_MinDist(Point, Segment);
bool Line_intersect(Segment , Segment);
bool On_Arc(Segment, Point);

void Scorer::open_file()
{
    Input_Output A;

    Q_file.open(INPUT_PATH, ios::in);
    A_file.open(OUTPUT_PATH, ios::in);
    string assemblygap_str, coppergap_str, silkscreenlen_str;
    Q_file >> assemblygap_str >> coppergap_str >> silkscreenlen_str;
    assemblygap = A.File_to_Parameter(assemblygap_str);
    coppergap = A.File_to_Parameter(coppergap_str);
    silkscreenlen = A.File_to_Parameter(silkscreenlen_str);
    assembly = A.Read_Assembly(Q_file);
    copper = A.Read_Copper(Q_file);
    silkscreen = Read_Silkscreen(A_file);
}

// untest 2022/7/7
double Scorer::first_quarter(const vector<Segment> Assembly, const vector<Segment> silkscreen)
{
    Input_Output A;
    float Rectangular_area = 0;       // 絲印標示之座標極限值所構成之矩形面積
    float X_max, Y_max, X_min, Y_min; //絲印座標極限值

    float Y_area = 0; // 零件外觀向外等比拓展Y之面積範圍
    vector<Segment> Assembly_push_out;

    float Answer_1;

    /* calculate Rectangular_area */
    X_max = silkscreen[0].x1;
    X_min = silkscreen[0].x2;
    Y_max = silkscreen[0].y1;
    Y_min = silkscreen[0].y2;

    for (int i = 0; i < silkscreen.size(); i++)
    {
        if (silkscreen[i].x2 > X_max)
            X_max = silkscreen[i].x2;
        if (silkscreen[i].x1 > X_max)
            X_max = silkscreen[i].x1;
        if (silkscreen[i].x2 < X_min)
            X_min = silkscreen[i].x2;
        if (silkscreen[i].x1 < X_min)
            X_min = silkscreen[i].x1;

        if (silkscreen[i].y2 > Y_max)
            Y_max = silkscreen[i].y2;
        if (silkscreen[i].y1 > Y_max)
            Y_max = silkscreen[i].y1;
        if (silkscreen[i].y2 < Y_min)
            Y_min = silkscreen[i].y2;
        if (silkscreen[i].y1 < Y_min)
            Y_min = silkscreen[i].y1;
    }
    Rectangular_area = abs((X_max - X_min) * (Y_max - Y_min));

    /* calculate Y_area*/
    vector<Point> Assembly_push_out_points;
    vector<vector<Point>> Arc_Dots;
    Assembly_push_out = A.Assembly_Buffer(Assembly, coppergap, assemblygap);
    Assembly_push_out_points = A.Line_to_Point(Assembly_push_out);
    Arc_Dots = A.Arc_Optimization(Assembly_push_out);

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
    double theta;     // Law of cosines

    float s; // heron formula

    float cut_area;

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

            radius = dis2(center_of_circle, point_1);
            c = dis2(point_1, point_2);
            s = (radius + radius + c) / 2;
            theta = acos((2 * (radius * radius) - c * c) / (2 * radius * c)); // Law of cosines

            cut_area = radius * radius * theta / 2 - sqrt(s * (s - radius) * (s - radius) * (s - c)); // last part is heron formula

            outside = !A.point_in_polygon(center_of_circle, Assembly_push_out_points, Arc_Dots);
            if (outside)
                total_area -= cut_area;
            else
                total_area += cut_area;
        }
    }

    Answer_1 = (2 - Rectangular_area / Y_area) * 0.25;
    cout << "Rectangular_area:" << Rectangular_area << endl;
    cout << "Y_area:" << Y_area << endl;
    cout << "Answer_1:" << Answer_1 <<endl;
    return Answer_1;
}

double Scorer::third_quarter(const vector<vector<Segment>> copper, const vector<Segment> silkscreen)
{
    // Input_Output A;
    float L_copper = assemblygap;
    double min_distance;
    double min_distance_sum;
    double T_copper;
    double Third_Score;
    Point A1, A2, B1, B2; // A,B兩線段

    min_distance_sum = 0;

    for (int i = 0; i < silkscreen.size(); i++)
    {
        A1.x = silkscreen[i].x1;
        A1.y = silkscreen[i].y1;
        A2.x = silkscreen[i].x2;
        A2.y = silkscreen[i].y2;
        for (int j = 0; j < copper.size(); j++)
        {
            for (int k = 0; k < copper.at(j).size(); k++)
            {
                B1.x = copper.at(j).at(k).x1;
                B1.y = copper.at(j).at(k).y1;
                B2.x = copper.at(j).at(k).x2;
                B2.y = copper.at(j).at(k).y1;
                if (silkscreen[i].is_line == 1 && copper.at(j).at(k).is_line == 1)
                {
                    if (dir(A1, A2, B1) * dir(A1, A2, B2) <= 0 && dir(B1, B2, A1) * dir(B1, B2, A2) <= 0) //兩線段相交, 距離為0
                        min_distance = 0;
                    else //如不相交, 最短距離為四個端點中到另一條線段距離的最小值
                        min_distance = min(min(min(disMin(A1, A2, B1), disMin(A1, A2, B2)), disMin(B1, B2, A1)), disMin(B1, B2, A2));
                }
                else if (silkscreen[i].is_line == 1 && copper.at(j).at(k).is_line == 0)
                {
                }
                else if (silkscreen[i].is_line == 0 && copper.at(j).at(k).is_line == 1)
                {
                }
                else if (silkscreen[i].is_line == 0 && copper.at(j).at(k).is_line == 0)
                {
                }
            }
        }
        min_distance_sum += min_distance;
    }
    T_copper = min_distance_sum / silkscreen.size();
    Third_Score = (1 - (T_copper - L_copper) * 10 / L_copper) * 0.25;
    return Third_Score;
}

double Scorer::fourth_quarter(const vector<Segment> Assembly, const vector<Segment> silkscreen)
{
    // Input_Output A;
    float L_outline = assemblygap;
    double min_distance;
    double min_distance_sum;
    double T_outline;
    double Fourth_Score;
    Point A1, A2, B1, B2; // A,B兩線段
    double rA = 0;
    double rB = 0;
    Point circle_center_A, circle_center_B;
    min_distance_sum = 0;

    for (int i = 0; i < silkscreen.size(); i++)
    {
        A1.x = silkscreen[i].x1;
        A1.y = silkscreen[i].y1;
        A2.x = silkscreen[i].x2;
        A2.y = silkscreen[i].y2;
        circle_center_A.x = silkscreen[i].center_x;
        circle_center_A.y = silkscreen[i].center_y;
        rA = dis2(A1, circle_center_A);
        for (int j = 0; j < Assembly.size(); j++)
        {
            B1.x = Assembly[i].x1;
            B1.y = Assembly[i].y1;
            B2.x = Assembly[i].x2;
            B2.y = Assembly[i].y2;
            circle_center_B.x = Assembly[i].center_x;
            circle_center_B.y = Assembly[i].center_y;
            rB = dis2(B1, circle_center_B);
            if (silkscreen[i].is_line == 1 && Assembly[j].is_line == 1)
            {
                if (dir(A1, A2, B1) * dir(A1, A2, B2) <= 0 && dir(B1, B2, A1) * dir(B1, B2, A2) <= 0) //兩線段相交, 距離為0
                    min_distance = 0;
                else //如不相交, 最短距離為四個端點中到另一條線段距離的最小值
                    min_distance = min(min(min(disMin(A1, A2, B1), disMin(A1, A2, B2)), disMin(B1, B2, A1)), disMin(B1, B2, A2));
            }
            else if (silkscreen[i].is_line == 1 && Assembly[j].is_line == 0)
            {
                min_distance = min(min(disMin(A1, A2, B1), disMin(A1, A2, B2)), (disMin(A1, A2, circle_center_B) - rB));
            }
            else if (silkscreen[i].is_line == 0 && Assembly[j].is_line == 1)
            {
                min_distance = min(min(disMin(B1, B2, A1), disMin(B1, B2, A2)), (disMin(B1, B2, circle_center_A) - rA));
            }
            else if (silkscreen[i].is_line == 0 && Assembly[j].is_line == 0)
            {
            }
        }
        min_distance_sum += min_distance;
    }
    L_outline = min_distance_sum / silkscreen.size();
    Fourth_Score = (1 - (T_outline - L_outline) * 10 / L_outline) * 0.25;
    return Fourth_Score;
}

vector<Segment> Scorer::Read_Silkscreen(fstream &Input_File)
{
    Input_Output A;
    vector<Segment> Assembly;
    Segment part;
    //vector<string> split_return;
    string line;
    getline(Input_File, line);

    while (getline(Input_File, line))
    {
        if (line == "assembly")
            continue;
        else
            part = A.String_to_Line(line);
        Assembly.push_back(part);
    }
    return Assembly;
}

double cross1(Point o, Point a, Point b)
{
    return (a.x-o.x) * (b.y-o.y) - (a.y-o.y) * (b.x-o.x);
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

Point operator + (Point a, Point b)
{
    Point v;
    v.x = a.x + b.x;
    v.y = a.y + b.y;
    return v;
}

Point operator * (const int c, Point a)
{
    Point v;
    v.x = c*a.x;
    v.y = c*a.y;
    return v;
}

Point operator / (Point a, const int c)
{
    Point v;
    v.x = a.x/c;
    v.y = a.y/c;
    return v;
}

double Point_to_Arc_MinDist(Point pp, Segment Arc)
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
}

vector<Point> intersection_between_CentersLine_and_Arc(Segment Arc, Point Center) //the other arc's center
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

    theta = atan2(centerpoint.y - Center.y, centerpoint.x + Center.x);
    v1 = Center - centerpoint;
    tmp.x = centerpoint.x + radius*cos(theta);
    tmp.y = centerpoint.y + radius*sin(theta);

    if(On_Arc(Arc, tmp))
    {
        intersect.push_back(tmp);
    }

    v2 = centerpoint - tmp;
    tmp = tmp + 2*v2;

    if(On_Arc(Arc, tmp))
    {
        intersect.push_back(tmp);
    }
    return intersect;
} 

Point find_arbitary_point_on_arc(Segment Arc){

    Point middlepoint; //A,B中點
    Point centerpoint, A, B;
    Point v1, v2;
    double radius;
    
    A.x = Arc.x1;
    A.y = Arc.y1;
    B.x = Arc.x2;
    B.y = Arc.y2;
    radius = dist(A,centerpoint);
    middlepoint = (A + B) / 2;
    centerpoint.x = Arc.center_x;
    centerpoint.y = Arc.center_y;
    
    v1 = centerpoint - middlepoint;
    v2 = B - A; 

    if((cross(v2, v1) > 0 && Arc.direction == 1) || (cross(v2, v1) < 0 && Arc.direction == 0))
        return middlepoint + (radius - dist(middlepoint,centerpoint))/dist(middlepoint,centerpoint)*(-1*v1);
    else
        return middlepoint + (dist(middlepoint,centerpoint) + radius)/dist(middlepoint,centerpoint)*v1;
}

bool On_Arc(Segment Arc, Point p)
{
    Segment AB, BC, OP;
    Point ar_p;
    ar_p = find_arbitary_point_on_arc(Arc);
    AB.x1 = Arc.x1;
    AB.y1 = Arc.y1;
    AB.x2 = ar_p.x = BC.x1;
    AB.y2 = ar_p.y = BC.y1;
    BC.x2 = Arc.x2;
    BC.y2 = Arc.y2;

    OP.x1 = Arc.center_x;
    OP.y1 = Arc.center_y;
    OP.x2 = ar_p.x;
    OP.y2 = ar_p.y;

    if(Line_intersect(AB, OP) || Line_intersect(BC, OP))
        return true;
    else
        return false;
}

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
    if (c1 * c2 < 0 && c3 * c4 < 0) return true;
    // 端點共線
    /*if (c1 == 0 && Line_intersect(a1, a2, b1)) return true;
    if (c2 == 0 && Line_intersect(a1, a2, b2)) return true;
    if (c3 == 0 && Line_intersect(b1, b2, a1)) return true;
    if (c4 == 0 && Line_intersect(b1, b2, a2)) return true;*/
    return false;
}


/*struct boarder
{
    float x1{-100000};
    float x2{-100000};
    float y1{-100000};
    float y2{-100000};

    // below only for "arc", set all zero for "line"
    float center_x{-100000}, center_y{-100000};
    bool direction{0}; // 0 = ClockWise(CW), 1 = ConterClockwise(CCW)
};*/

/*vector<boarder> *extended_y() // to extend the assembly with length of "y"
{
}*/
/*int first_quarter() // Raymond
{
    return 0;
}

int second_quarter() // Willy Shius
{
    return 0;
}

int third_quarter() // peter // find nearest silkscreen for each copper and take average value
{
    return 0;
}

int fourth_quarter() // macoto //絲印標示與零件外觀之平均距離評分
{
    return 0;
}

int main()
{
    /* ----------Variable declaration---------- */
/*int first_score, second_score, third_score, fourth_score, total_score;
static string input_file_path = "input.txt";   // the given IC path
static string output_file_path = "output.txt"; // silkscreen path
float assemblygap, coppergap, silkscreenlen;   // read the input, given by input.txt()
boarder temp_boarder;

ifstream input_file, output_file;
list<boarder> assembly;
list<list<boarder>> copper;*/

// vector<boarder> *copper = new vector<boarder>[5];     //  unused

/* ------End of Variable declaration------- */

/* -------------Reading files-------------- */

//!!!not finished!!!
// input_file.open(input_file_path, ios::in);
// output_file.open(output_file_path, ios::in);
/*@TODO
    !!function needed!! *split the input string*


    string temp;
    input_file >> temp
    if(temp == "assemblygap")
    {
        define assemblygap
    }else if(temp == "coppergap"){
        define coppergap
    }else if(temp == "silkscreenlen"){
        define silkscreenlen
    }else if(temp == "assembly"){
        switch current input object to assembly
    }else if (temp == "copper"){
        switch input object to copper / to next copper

    }else if (temp == "line"){
        file >> temp_boarder;
        list current.push_back(temp_boarder);
    }else if (temp == "arc"){
        file >> temp_boarder;
        list current.push_back(temp_boarder);

    }else{
        cerr << "Error!";
    }

*/

/* ----------End of Reading Files---------- */

/* -----------Calling Functions------------ */
/*first_score = first_quarter();
second_score = second_quarter();
third_score = third_quarter();
fourth_score = fourth_quarter();
/* --------End of Calling Functions-------- */

// total_score = first_score + second_score + third_score + fourth_score;

/* --------------Score output-------------- */
/*cout << "first_score: " << first_score << endl
     << "second_score: " << second_score << endl
     << "third_score: " << third_score << endl
     << "fourth_score: " << fourth_score << endl;
cout << "The score of this silkscreen is: " << total_score << " / 100" << endl;*/
/* -----------End of Score output---------- */
//}