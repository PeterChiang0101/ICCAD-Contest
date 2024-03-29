#include <algorithm>
#include <cmath>
#include <cstring> // strtok
#include <fstream>
#include <iomanip> //setprecision & fixed
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

#define Angle_Tolerance 0.1           //算角度誤差容許值 (rad)
#define Subtraction_Tolerance 0.00005 // float 相減誤差容許值
#define PI 3.14159265358979323846
#define ARC_TO_LINE_SLICE_DENSITY 1 // 切片密度(in degree)
#define INPUT_PATH "./TestingCase/test_B.txt"
#define OUTPUT_PATH "./TestingCase/test_B_Ans.txt"
// assemblygap : the minimum distance between assembly and silkscreen
// coppergap : the minimum distance between copper and silkscreen
// silkscreenlen : the minimum length of silkscreen
float assemblygap, coppergap, silkscreenlen;

struct Segment
{
    bool is_line; // 0 = arc, 1 = line
    float x1;
    float y1;
    float x2;
    float y2;
    float slope;       //斜率
    float y_intercept; //截距
    double theta;      // the angle reference to positive x axis
    // below is needed by arc, when deals with line set all to 0
    float center_x;
    float center_y;
    bool direction; // 0 = ClockWise(CW), 1 = ConterClockwise(CCW)
    double theta_1; // 圓心到點一角度
    double theta_2; // 圓心到點二角度
};

struct Point
{
    float x;
    float y;
    bool Next_Arc; // if the point connected to arc
};

struct Copper // 外擴Copper
{
    float x_min, x_max, y_min, y_max;
    vector<Segment> segment;
};

///////////////////////////////////////////////////////////////////////////////////
////////////////////////////// functions declaration //////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

float File_to_Parameter(const string);

const vector<string> split(const string &, const char &);

Segment String_to_Line(const string);

vector<Segment> Read_Assembly(fstream &);

vector<vector<Segment>> Read_Copper(fstream &);

vector<Point> Line_to_Point(const vector<Segment>); //將線段切割成點

vector<Segment> Assembly_Buffer(const vector<Segment>);

vector<Copper> Copper_Buffer(const vector<vector<Segment>>);

vector<Point> Point_Extension(const vector<Segment>, const bool);

vector<Segment> Point_to_Line(vector<Point>, vector<Segment>);

Copper Copper_Point_to_Line(vector<Point>, vector<Segment>);

float interpolate_x(const float, const Point, const Point);

bool point_in_polygon(const Point, const vector<Point>, const vector<vector<Point>>);

vector<Segment> silkscreen_cut_single_copper(Segment, Copper);

vector<Segment> Cut_Silkscreen_by_Copper(const Segment, const vector<Copper>);

vector<Segment> Segment_Sort(Segment, vector<Segment>);

bool sort_decrease_Segment(const Segment, const Segment);

bool sort_increase_Segment(const Segment, const Segment);

vector<Point> Point_Sort(vector<Point>);

bool sort_decrease_points(const Point, const Point);

bool sort_increase_points(const Point, const Point);

vector<Segment> Final_Silkscreen(const vector<Segment>, const vector<Copper>);

void Write_File(const vector<Segment>);

void Write_File(const vector<vector<Segment>>, char **);

void Write_File_Copper(const vector<Copper>); // debugging function

vector<Point> Arc_to_Poly(Segment);

vector<vector<Point>> Arc_Optimization(const vector<Segment>);

Copper Arc_Boundary_Meas(Segment);

float cross(Point, Point);

Point intersection(Point, Point, Point, Point);

bool In_Between_Lines(Point, Point, Point);

vector<vector<Segment>> Delete_Short_Silkscreen(vector<Segment>);

vector<vector<Segment>> Find_Continuous_Segment(vector<Segment>);
// bool Outside_of_Assembly(const Point, const vector<Segment>);

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// main functions //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) // safe
{
    fstream file;
    string assemblygap_str, coppergap_str, silkscreenlen_str;

    file.open(argv[1], ios::in);

    // the first three line of the file, defines parameters for silkscreen
    file >> assemblygap_str >> coppergap_str >> silkscreenlen_str;
    assemblygap = File_to_Parameter(assemblygap_str);
    coppergap = File_to_Parameter(coppergap_str);
    silkscreenlen = File_to_Parameter(silkscreenlen_str);

    vector<Segment> assembly;
    vector<vector<Segment>> copper;
    vector<Segment> silkscreen;
    assembly = Read_Assembly(file);
    copper = Read_Copper(file);
    // checked
    // cout << assembly.size() << " " << copper.size() << endl;

    silkscreen = Assembly_Buffer(assembly);
    // checked
    vector<Copper> whole_copper_barrier;
    whole_copper_barrier = Copper_Buffer(copper);
    vector<Segment> Silkscreen_Cut;

    vector<vector<Segment>> Continuous_Silkscreen; // 連續線段在同一vector裡
    // Write_File(silkscreen);
    Silkscreen_Cut = Final_Silkscreen(silkscreen, whole_copper_barrier);

    Continuous_Silkscreen = Delete_Short_Silkscreen(Silkscreen_Cut);

    Write_File(Continuous_Silkscreen, argv);

    Write_File(Silkscreen_Cut);

    Write_File_Copper(whole_copper_barrier); // output for testing

    // calculate the silkscreen
    // ignore the arc first

    // arc needed to be treated manually

    // output
}

float File_to_Parameter(const string str) // 讀入參數 safe
{
    string str_truncate;
    str_truncate = str.substr(str.find(',') + 1);
    return stof(str_truncate);
}

const vector<string> split(const string &str, const char &delimiter) // 拆分文字 safe
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

Segment String_to_Line(string line) // 讀取時建立線段 safe
{
    vector<string> Splited;
    Splited = split(line, ',');
    int vector_size = Splited.size();
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
        // theta_1 = 0 , theta_2 = 0 ???
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

vector<Segment> Read_Assembly(fstream &Input_File) // 讀取assembly，轉換為vector safe
{
    vector<Segment> Assembly;
    Segment part;
    vector<string> split_return;
    string line;
    getline(Input_File, line);

    while (getline(Input_File, line))
    {
        if (line[0] == 'c')
            break;
        else if (line[1] == 's')
            continue;
        else
            part = String_to_Line(line);
        Assembly.push_back(part);
    }
    return Assembly;
}

vector<vector<Segment>> Read_Copper(fstream &Input_File) // 讀取copper，轉換為二維vector safe
{
    vector<Segment> copper;
    vector<vector<Segment>> copper_pack;
    Segment part;
    vector<string> split_return;
    string line;
    while (getline(Input_File, line))
    {
        if (line[0] == 'c')
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

vector<Point> Line_to_Point(const vector<Segment> Assembly) // 將線段切割成點 assume safe
{
    const int size = Assembly.size();
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

vector<Segment> Assembly_Buffer(const vector<Segment> Assembly) // safe
{
    vector<Point> Extended_Points = Point_Extension(Assembly, true);
    vector<Segment> silkscreen = Point_to_Line(Extended_Points, Assembly);
    return silkscreen;
}

vector<Copper> Copper_Buffer(const vector<vector<Segment>> coppers) // safe
{
    int size = coppers.size();
    Copper Single_Copper;
    vector<Copper> Every_Copper;
    vector<Point> Extended_Points;
    for (int i = 0; i < size; i++)
    {
        Extended_Points = Point_Extension(coppers.at(i), false);
        // problem!!!!!!
        //cout<<Extended_Points.size()<<" "<<Extended_Points<<endl;
        Single_Copper = Copper_Point_to_Line(Extended_Points, coppers.at(i));
        Every_Copper.push_back(Single_Copper);
    }
    return Every_Copper;
}

vector<Point> Point_Extension(const vector<Segment> Assembly, const bool is_assembly) // 圖形外擴
{
    const int size = Assembly.size();
    //cout << size << endl;
    vector<Point> Assembly_Points;
    vector<Point> Extended_Points;
    vector<vector<Point>> Arc_Dots;
    vector<Segment> Silkscreen;

    Assembly_Points = Line_to_Point(Assembly); //線切割為點
    Arc_Dots = Arc_Optimization(Assembly);     // 將圓弧切割成多個點，以利辨識點在圖形內外
    //cout<<Arc_Dots<<endl;
    if (size == 1) // i think only happened in copper, eg: a full circle
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

vector<Segment> Point_to_Line(vector<Point> Extended_Points, vector<Segment> Assembly) // assume not safe
{
    int size = Assembly.size();

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
        Silkscreen.push_back(A_Line);
    }
    return Silkscreen;
}

Copper Copper_Point_to_Line(vector<Point> Extended_Points, vector<Segment> copper) // not safe
{
    int size = copper.size();
    Segment A_Line;
    Copper Silkscreen, Arc_Boundary;
    if (!Extended_Points.empty())
    {
        Silkscreen.x_min = Extended_Points.at(0).x;
        Silkscreen.x_max = Extended_Points.at(0).x; // initialize
        Silkscreen.y_min = Extended_Points.at(0).y;
        Silkscreen.y_max = Extended_Points.at(0).y;
    }

    for (size_t i = 0; i < size; i++)
    {
        // calculate boundary
        if (!copper.at(i).is_line)
        {
            Arc_Boundary = Arc_Boundary_Meas(copper.at(i));
            Silkscreen.x_min = min(Silkscreen.x_min, Arc_Boundary.x_min);
            Silkscreen.x_max = max(Silkscreen.x_max, Arc_Boundary.x_max);
            Silkscreen.y_min = min(Silkscreen.y_min, Arc_Boundary.y_min);
            Silkscreen.y_max = max(Silkscreen.y_max, Arc_Boundary.y_max);
        }
        if (Extended_Points.at(i).x > Silkscreen.x_max)
        {
            Silkscreen.x_max = Extended_Points.at(i).x;
        }
        else if (Extended_Points.at(i).x < Silkscreen.x_min)
        {
            Silkscreen.x_min = Extended_Points.at(i).x;
        }

        if (Extended_Points.at(i).y > Silkscreen.y_max)
        {
            Silkscreen.y_max = Extended_Points.at(i).y;
        }
        else if (Extended_Points.at(i).y < Silkscreen.y_min)
        {
            Silkscreen.y_min = Extended_Points.at(i).y;
        }

        // calculate point to line
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

            A_Line.center_x = copper.at(i).center_x;
            A_Line.center_y = copper.at(i).center_y;
            A_Line.direction = copper.at(i).direction;

            A_Line.theta_1 = atan2(A_Line.y1 - A_Line.center_y, A_Line.x1 - A_Line.center_x);
            A_Line.theta_2 = atan2(A_Line.y2 - A_Line.center_y, A_Line.x2 - A_Line.center_x);
        }
        Silkscreen.segment.push_back(A_Line);
    }

    return Silkscreen;
}

float interpolate_x(float y, Point p1, Point p2) // 待測點與圖形邊界交會的x值 safe
{
    if (p1.y == p2.y)
        return p1.x;
    return p1.x + (p2.x - p1.x) * (y - p1.y) / (p2.y - p1.y);
}

bool point_in_polygon(Point t, vector<Point> Assembly_Point, vector<vector<Point>> Arc_Points) // 運用射線法判斷點在圖形內外 assume safe
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

void Write_File(const vector<Segment> Silkscreen) // safe
{
    fstream Output;

    Output.open(OUTPUT_PATH, ios::out);
    // Output << "silkscreen" << endl;
    const int size = Silkscreen.size();
    for (size_t i = 0; i < size; i++)
    {
        if (i == 0 || Silkscreen.at(i).x1 != Silkscreen.at(i - 1).x2 || Silkscreen.at(i).y1 != Silkscreen.at(i - 1).y2) // 第一條線，或線段不連續
            Output << "silkscreen" << endl;

        if (Silkscreen.at(i).is_line)
        {
            Output << "line," << fixed << setprecision(4) << Silkscreen.at(i).x1 << "," << Silkscreen.at(i).y1 << "," << Silkscreen.at(i).x2 << "," << Silkscreen.at(i).y2 << endl;
        }
        else
        {
            Output << "arc," << fixed << setprecision(4) << Silkscreen.at(i).x1 << "," << Silkscreen.at(i).y1 << "," << Silkscreen.at(i).x2 << "," << Silkscreen.at(i).y2 << "," << Silkscreen.at(i).center_x << "," << Silkscreen.at(i).center_y << "," << (Silkscreen.at(i).direction ? "CCW" : "CW") << endl;
        }
    }
}

void Write_File(const vector<vector<Segment>> Silkscreen, char **argv) // safe
{
    fstream Output;

    Output.open(argv[2], ios::out);

    const int size = Silkscreen.size();
    for (size_t i = 0; i < size; i++)
    {
        Output << "silkscreen" << endl;
        int conti_size = Silkscreen.at(i).size();
        for (int j = 0; j < conti_size; j++)
        {
            if (Silkscreen.at(i).at(j).is_line)
            {
                Output << "line," << fixed << setprecision(4) << Silkscreen.at(i).at(j).x1 << "," << Silkscreen.at(i).at(j).y1 << "," << Silkscreen.at(i).at(j).x2 << "," << Silkscreen.at(i).at(j).y2 << endl;
            }
            else
            {
                Output << "arc," << fixed << setprecision(4) << Silkscreen.at(i).at(j).x1 << "," << Silkscreen.at(i).at(j).y1 << "," << Silkscreen.at(i).at(j).x2 << "," << Silkscreen.at(i).at(j).y2 << "," << Silkscreen.at(i).at(j).center_x << "," << Silkscreen.at(i).at(j).center_y << "," << (Silkscreen.at(i).at(j).direction ? "CCW" : "CW") << endl;
            }
        }
    }
}

vector<Point> Arc_to_Poly(Segment Arc) // not safe
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
        int last_element = Poly_out.size() - 1;

        if (Poly_out.at(last_element).x != Arc.x2 && Poly_out.at(last_element).y != Arc.y2)
        {
            part.x = Arc.x2;
            part.y = Arc.y2;
            part.Next_Arc = false;
            Poly_out.push_back(part);
        }
    }

    return Poly_out;
}

vector<vector<Point>> Arc_Optimization(vector<Segment> Assembly) // safe
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

Copper Arc_Boundary_Meas(Segment Arc) // safe
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

void Write_File_Copper(const vector<Copper> coppers) // no use
{
    int size = coppers.size();
    fstream Output;

    Output.open(OUTPUT_PATH, ios::app);
    for (int i = 0; i < size; i++)
    {
        int copper_line = coppers.at(i).segment.size();
        for (int j = 0; j < copper_line; j++)
        {
            if (coppers.at(i).segment.at(j).is_line)
            {
                Output << "line," << fixed << setprecision(4) << coppers.at(i).segment.at(j).x1 << "," << coppers.at(i).segment.at(j).y1 << "," << coppers.at(i).segment.at(j).x2 << "," << coppers.at(i).segment.at(j).y2 << endl;
            }
            else
            {
                Output << "arc," << fixed << setprecision(4) << coppers.at(i).segment.at(j).x1 << "," << coppers.at(i).segment.at(j).y1 << "," << coppers.at(i).segment.at(j).x2 << "," << coppers.at(i).segment.at(j).y2 << "," << coppers.at(i).segment.at(j).center_x << "," << coppers.at(i).segment.at(j).center_y << "," << (coppers.at(i).segment.at(j).direction ? "CCW" : "CW") << endl;
            }
        }
    }
}

vector<Segment> Final_Silkscreen(vector<Segment> Silkscreen_Original, vector<Copper> Coppers) // 未切割的絲印 與 銅箔 safe
{
    vector<Segment> Silkscreen_Cut_Complete; // 切割完成的完整絲印
    vector<Segment> Silkscreen_Cut_Part;     // 切割完成的一條絲印
    int Silkscreen_Org_Size = Silkscreen_Original.size();

    for (int i = 0; i < Silkscreen_Org_Size; i++)
    {
        Silkscreen_Cut_Part.clear();
        Silkscreen_Cut_Part = Cut_Silkscreen_by_Copper(Silkscreen_Original.at(i), Coppers);
        Silkscreen_Cut_Complete.insert(Silkscreen_Cut_Complete.end(), Silkscreen_Cut_Part.begin(), Silkscreen_Cut_Part.end());
    }
    return Silkscreen_Cut_Complete;
}

vector<Segment> Cut_Silkscreen_by_Copper(Segment Silkscreen_Piece, vector<Copper> Coppers) // safe
{
    int Copper_size = Coppers.size();
    vector<Segment> Single_Silkscreen_Cut_Complete;
    vector<Segment> total_copper_cut_segments; // 取所有須切割區域的聯集
    vector<Segment> copper_cut_segments;       // 一個copper所遮住這條絲印的部分
    Segment A_Line;

    float x_min = min(Silkscreen_Piece.x1, Silkscreen_Piece.x2);
    float x_max = max(Silkscreen_Piece.x1, Silkscreen_Piece.x2);
    float y_min = min(Silkscreen_Piece.y1, Silkscreen_Piece.y2);
    float y_max = max(Silkscreen_Piece.y1, Silkscreen_Piece.y2);

    // Single_Silkscreen_Cut_Complete.push_back(Silkscreen_Piece);
    for (int i = 0; i < Copper_size; i++) // 每次處理一個copper
    {
        if (x_min > Coppers.at(i).x_max || x_max < Coppers.at(i).x_min || y_min > Coppers.at(i).y_max || y_max < Coppers.at(i).y_min)
            continue;
        copper_cut_segments = silkscreen_cut_single_copper(Silkscreen_Piece, Coppers.at(i));                                       // 絲印與單一copper的交集線段
        total_copper_cut_segments.insert(total_copper_cut_segments.end(), copper_cut_segments.begin(), copper_cut_segments.end()); // 線段之間可能有交集
    }
    total_copper_cut_segments = Segment_Sort(Silkscreen_Piece, total_copper_cut_segments);

    int total_segment = total_copper_cut_segments.size(); // 聯集完

    if (total_segment == 2) // 只有頭跟尾，不需要切割
    {
        Single_Silkscreen_Cut_Complete.push_back(Silkscreen_Piece);
        return Single_Silkscreen_Cut_Complete;
    }
    for (int i = 1; i < total_segment; i++)
    {
        if (total_copper_cut_segments.at(i).x1 == total_copper_cut_segments.at(i - 1).x1 && total_copper_cut_segments.at(i).y1 == total_copper_cut_segments.at(i - 1).y2) // 共點
            continue;
        A_Line.x1 = total_copper_cut_segments.at(i - 1).x2;
        A_Line.y1 = total_copper_cut_segments.at(i - 1).y2;
        A_Line.x2 = total_copper_cut_segments.at(i).x1;
        A_Line.y2 = total_copper_cut_segments.at(i).y1;
        Single_Silkscreen_Cut_Complete.push_back(A_Line); // 最終切完的結果
    }
    return Single_Silkscreen_Cut_Complete;
}

vector<Segment> silkscreen_cut_single_copper(Segment Silkscreen_Piece, Copper Single_Copper) // safe
{
    int Copper_Line_size = Single_Copper.segment.size();
    vector<vector<Point>> Arc_Dots;
    vector<Point> Copper_Points;
    vector<Point> Intersection_Points;
    Point first_point, last_point;
    Point Point_Intersect;
    bool first_inside, last_inside;

    first_point.x = Silkscreen_Piece.x1;
    first_point.y = Silkscreen_Piece.y1;
    last_point.x = Silkscreen_Piece.x2;
    last_point.y = Silkscreen_Piece.y2;

    Copper_Points = Line_to_Point(Single_Copper.segment);
    Arc_Dots = Arc_Optimization(Single_Copper.segment);
    //cout<<Arc_Dots.size()<<endl;
    first_inside = point_in_polygon(first_point, Copper_Points, Arc_Dots);
    last_inside = point_in_polygon(last_point, Copper_Points, Arc_Dots);

    Intersection_Points.push_back(first_point);
    for (int i = 0; i < Copper_Line_size; i++)
    {
        Point first_copper_point, second_copper_point;
        first_copper_point.x = Single_Copper.segment.at(i).x1;
        first_copper_point.y = Single_Copper.segment.at(i).y1;
        second_copper_point.x = Single_Copper.segment.at(i).x2;
        second_copper_point.y = Single_Copper.segment.at(i).y2;
        Point_Intersect = intersection(first_point, last_point, first_copper_point, second_copper_point); // 交會點
        if (Point_Intersect.x != INFINITY && Point_Intersect.y != INFINITY)
            Intersection_Points.push_back(Point_Intersect);
    }
    Intersection_Points.push_back(last_point);

    Intersection_Points = Point_Sort(Intersection_Points);

    vector<Segment> Cut_Lines;
    Segment A_Line;
    int Intersection_Points_size = Intersection_Points.size();

    for (size_t i = 0; i < Intersection_Points_size; i++) // 量出需要被切割的線段
    {
        if ((i == 0 && !first_inside) || (i == Intersection_Points_size - 1 && !last_inside))
            continue; // 兩端點在外面可以直接略過
        A_Line.x1 = Intersection_Points.at(i).x;
        A_Line.y1 = Intersection_Points.at(i).y;
        i++;
        A_Line.x2 = Intersection_Points.at(i).x;
        A_Line.y2 = Intersection_Points.at(i).y;
        Cut_Lines.push_back(A_Line);
    }

    return Cut_Lines;
}

// 叉積運算，回傳純量（除去方向）
float cross(Point v1, Point v2) // 向量外積 safe
{
    // 沒有除法，儘量避免誤差。
    return v1.x * v2.y - v1.y * v2.x;
}

Point intersection(Point a1, Point a2, Point b1, Point b2) // safe
{
    Point a, b, s;
    a.x = a2.x - a1.x, a.y = a2.y - a1.y;
    b.x = b2.x - b1.x, b.y = b2.y - b1.y;
    s.x = b1.x - a1.x, s.y = b1.y - a1.y;

    // 兩線平行，交點不存在。
    // 兩線重疊，交點無限多。
    if (cross(a, b) == 0)
    {
        s.x = s.y = INFINITY;
        return s;
    }

    // 計算交點

    a.x = a1.x + a.x * (cross(s, b) / cross(a, b));
    a.y = a1.y + a.y * (cross(s, b) / cross(a, b));

    if (In_Between_Lines(a, a1, a2) && In_Between_Lines(a, b1, b2))
        return a;
    else
    {
        s.x = s.y = INFINITY;
        return s;
    }
}

bool In_Between_Lines(Point test, Point first, Point last) // safe
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

vector<vector<Segment>> Find_Continuous_Segment(vector<Segment> Silkscreen) // safe
{
    vector<vector<Segment>> continue_segment;
    vector<Segment> continue_temp;
    size_t Silkscreen_size = Silkscreen.size();
    for (size_t i = 0; i < Silkscreen_size; i++)
    {
        if (i == Silkscreen_size - 1)
        {
            continue_temp.push_back(Silkscreen.at(i));

            if ((Silkscreen.at(i).x2 == Silkscreen.at(0).x1) && (Silkscreen.at(i).y2 == Silkscreen.at(0).y1))
            {
                if (!continue_segment.empty())
                {
                    continue_temp.insert(continue_temp.end(), continue_segment.at(0).begin(), continue_segment.at(0).end());
                    continue_segment.erase(continue_segment.begin());
                }
                else
                {
                    continue_temp.push_back(Silkscreen.at(i));
                }
            }
            continue_segment.push_back(continue_temp);
        }
        else
        {
            if ((Silkscreen.at(i).x2 == Silkscreen.at(i + 1).x1) && (Silkscreen.at(i).y2 == Silkscreen.at(i + 1).y1))
            {
                continue_temp.push_back(Silkscreen.at(i));
            }
            else
            {
                continue_temp.push_back(Silkscreen.at(i));
                continue_segment.push_back(continue_temp);
                // clear the continue_temp
                continue_temp.clear();
            }
        }
    }
    return continue_segment;
}

vector<vector<Segment>> Delete_Short_Silkscreen(vector<Segment> Silkscreen) // safe
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
///////////////////////////////////////////////////////
////////////////// sorting functions //////////////////
///////////////////////////////////////////////////////

vector<Segment> Segment_Sort(Segment Silkscreen_Piece, vector<Segment> total_copper_cut_segments) // safe
{
    // 由 x1 or y1 小至大排序
    // 開頭Segment 為 Silkscreen_Piece.x1, Silkscreen_Piece.y1, Silkscreen_Piece.x1, Silkscreen_Piece.y1
    // 排序
    // 結尾Segment 為 Silkscreen_Piece.x2, Silkscreen_Piece.y2, Silkscreen_Piece.x2, Silkscreen_Piece.y2
    vector<Segment> Cut_Silkscreen;
    Segment Start_point, End_point;
    Start_point.x1 = Start_point.x2 = Silkscreen_Piece.x1;
    Start_point.y1 = Start_point.y2 = Silkscreen_Piece.y1;

    End_point.x1 = End_point.x2 = Silkscreen_Piece.x2;
    End_point.y1 = End_point.y2 = Silkscreen_Piece.y2;
    Cut_Silkscreen.push_back(Start_point);
    if (Silkscreen_Piece.is_line)
    {
        if (Silkscreen_Piece.x1 < Silkscreen_Piece.x2)
        {
            sort(total_copper_cut_segments.begin(), total_copper_cut_segments.end(), sort_increase_Segment);
        }
        else if (Silkscreen_Piece.x1 > Silkscreen_Piece.x2)
        {
            sort(total_copper_cut_segments.begin(), total_copper_cut_segments.end(), sort_decrease_Segment);
        }
        else if (Silkscreen_Piece.y1 < Silkscreen_Piece.y2)
        {
            sort(total_copper_cut_segments.begin(), total_copper_cut_segments.end(), sort_increase_Segment);
        }
        else if (Silkscreen_Piece.y1 > Silkscreen_Piece.y2)
        {
            sort(total_copper_cut_segments.begin(), total_copper_cut_segments.end(), sort_decrease_Segment);
        }
        else
        { // ERROR STATUS X1=X2, Y1=Y2;sort_increase_Segment
            sort(total_copper_cut_segments.begin(), total_copper_cut_segments.end(), sort_increase_Segment);
            cerr << "ERROR STATUS points are the same" << endl;
        }
        /*---------------------------------------abandon decision function-------------------------
        if ((Silkscreen_Piece.theta >= 0 && Silkscreen_Piece.theta < PI / 2) || (Silkscreen_Piece.theta < 3 * PI / 2 && Silkscreen_Piece.theta > PI))
        { // SORT BY X1,increase
            sort(total_copper_cut_segments.begin(), total_copper_cut_segments.end(), sort_increase_Segement);
        }
        else if ((Silkscreen_Piece.theta > PI / 2 && Silkscreen_Piece.theta <= PI) || (Silkscreen_Piece.theta > 3 * PI / 2 && Silkscreen_Piece.theta < 2 * PI))
        { // SORT BY X1,decrease
            sort(total_copper_cut_segments.begin(), total_copper_cut_segments.end(), sort_decrease_Segment);
        }
        else if (Silkscreen_Piece.theta == PI / 2)
        {
            sort(total_copper_cut_segments.begin(), total_copper_cut_segments.end(), sort_increase_Segement);
        }
        else
        {
            sort(total_copper_cut_segments.begin(), total_copper_cut_segments.end(), sort_decrease_Segment);
        }
        ****************************************************************/
    }
    else
    {
    }
    Cut_Silkscreen.insert(Cut_Silkscreen.end(), total_copper_cut_segments.begin(), total_copper_cut_segments.end());
    Cut_Silkscreen.push_back(End_point);

    return Cut_Silkscreen;
}

bool sort_increase_Segment(const Segment L1, const Segment L2) // safe
{
    if (L1.x1 == L2.x1)
    {
        return (L1.y1 < L2.y1);
    }
    else
    {
        return (L1.x1 < L2.x1);
    }
}
bool sort_decrease_Segment(const Segment L1, const Segment L2) // safe
{
    if (L1.x1 == L2.x1)
    {
        return (L1.y1 > L2.y1);
    }
    else
    {
        return (L1.x1 > L2.x1);
    }
}

vector<Point> Point_Sort(vector<Point> Intersection_Points) // safe
{
    // 傳進vector，最少有兩個element at() 不越界
    // Warning!! this version will modify the input array and return it back.
    // vector<Point> sorted_Points;
    size_t final_point = Intersection_Points.size() - 1;

    if ((Intersection_Points.at(0).x) > (Intersection_Points.at(final_point).x))
    {
        sort(Intersection_Points.begin(), Intersection_Points.end(), sort_decrease_points);
    }
    else if ((Intersection_Points.at(0).x) < (Intersection_Points.at(final_point).x))
    {
        sort(Intersection_Points.begin(), Intersection_Points.end(), sort_increase_points);
    }
    else if ((Intersection_Points.at(0).y) < (Intersection_Points.at(final_point).y))
    {
        sort(Intersection_Points.begin(), Intersection_Points.end(), sort_increase_points);
    }
    else
    {
        sort(Intersection_Points.begin(), Intersection_Points.end(), sort_decrease_points);
    }

    return Intersection_Points;
}

bool sort_decrease_points(const Point p1, const Point p2) // safe
{
    if (p1.x != p2.x)
    {
        return (p1.x > p2.x);
    }
    else
    {
        return (p1.y > p2.y);
    }
}
bool sort_increase_points(const Point p1, const Point p2) // safe
{
    if (p1.x != p2.x)
    {
        return (p1.x < p2.x);
    }
    else
    {
        return (p1.y < p2.y);
    }
}

///////////////////////////////////////////////////////
///////////////// abandoned functions /////////////////
///////////////////////////////////////////////////////

/*
// 原因：原本想用線段外擴求焦點，因過於複雜放棄，改用點外擴 Point_Extension()

Segment line_offset(const Segment original_line, const float assemblygap) // not implemented
{
    float line_length;
    float x_offset, y_offset;
    Segment silkscreen;
    x_offset = abs(original_line.y1 - original_line.y2);
    y_offset = abs(original_line.x1 - original_line.x2);

    line_length = hypot(x_offset, y_offset);

    silkscreen.is_line = 1;
    silkscreen.center_x = silkscreen.center_y = silkscreen.direction = 0;
    silkscreen.x1 = original_line.x1 + x_offset / line_length;
    silkscreen.x2 = original_line.x2 + x_offset / line_length;
    silkscreen.y1 = original_line.y1 + y_offset / line_length;
    silkscreen.y2 = original_line.y2 + y_offset / line_length;

    return silkscreen;
}
*/

/*
vector<Point> Arc_to_Line(const vector<Segment> Assembly)
{
    const int size = Assembly.size();
    for (size_t i = 0; i < size; i++)
    {
        Segment new_line;
        Segment first_line, second_line;
        first_line = Assembly.at(i);
        if (i != size - 1)
            second_line = Assembly.at(i+1);
        else
            second_line = Assembly.at(0);
        if(!second_line.is_line)
        {
            new_line.slope = -1/second_line.slope;
            new_line.x1 = second_line.x1;
            new_line.y1 = second_line.y1;
            new_line.x2 = second_line.x1 + 1;
            new_line.y2 = second_line.y2 + new_line.slope;
            new_line.center_x = 0;
            new_line.center_y = 0;
            new_line.direction = 0;
            new_line.is_line = true;
            new_line.arc_tangent_line = true;
            new_line.y_intercept = new_line.y1 - new_line.slope * new_line.x1;
        }
    }
    //one arc will generate two lines
    //using vector.insert() to insert the second line
    //this action will modify the original data
    //the struct "Segment" need a extra bool to tell Silkscreen_Point_Extension the first and second line are forbidden to extrapolate
}
*/

/*
// 原因：此方法僅適用凸多邊形，Case C 會導致錯誤，改用射線法 point_in_polygon()

bool Outside_of_Assembly(const Point a, const vector<Segment> Assembly) //  使用角度相加為2 * PI，
{
    vector<Point> Assembly_Points = Line_to_Point(Assembly);
    vector<float> Angle_Vector; // 各點之間角度差
    float Angle;
    float First_Angle, Second_Angle;
    float Total_Angle = 0;

    int size = Assembly_Points.size();
    for (size_t i = 0; i < size; i++)
    {
        Angle_Vector.push_back(atan2(Assembly_Points.at(i).y - a.y, Assembly_Points.at(i).x - a.x));
    }
    for (size_t i = 0; i < size; i++)
    {
        First_Angle = Angle_Vector.at(i);
        if (i == size - 1)
            Second_Angle = Angle_Vector.at(0);
        else
            Second_Angle = Angle_Vector.at(i+1);
        Angle = abs(Second_Angle - First_Angle);       // 角度差
        Angle = (Angle > PI) ? 2 * PI - Angle : Angle; // 差值永遠為正
        Total_Angle += Angle;
    }
    if (abs(Total_Angle - 2 * PI) > Angle_Tolerance)
        return true;
    else
        return false;
}
*/

/*
// 原因：改變回傳資料型態，保留原本function

vector<Segment> Arc_to_Poly(Segment Arc)
{
    vector<Segment> Poly_out;
    Segment part;

    double theta_ref;
    double theta_in;
    double theta_div;
    double radius;
    int times;
    int count;

    theta_ref = Arc.theta_1;
    theta_div = 2 * PI / 360; // div/degree
    radius = sqrt((Arc.x1 - Arc.center_x) * (Arc.x1 - Arc.center_x) + (Arc.y1 - Arc.center_y) * (Arc.y1 - Arc.center_y));

    if (Arc.direction == 0) // CW
    {
        if (Arc.theta_1 - Arc.theta_2 < 0)
            theta_in = Arc.theta_1 - Arc.theta_2 + 2 * PI;
        else
            theta_in = Arc.theta_1 - Arc.theta_2;
    }
    else
    {
        if (Arc.theta_2 - Arc.theta_1 < 0)
            theta_in = Arc.theta_2 - Arc.theta_1 + 2 * PI;
        else
            theta_in = Arc.theta_2 - Arc.theta_1;
    }

    times = (int)(theta_in / (2 * PI) * 360);
    count = times;

    while (count > 0)
    {

        if (count == times)
        {
            part.x1 = Arc.x1;
            part.y1 = Arc.y1;
        }
        else
        {
            part.x1 = Poly_out.at(times - count - 1).x2;
            part.y1 = Poly_out.at(times - count - 1).y2;
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

        part.x2 = Arc.center_x + radius * cos(theta_ref);
        part.y2 = Arc.center_y + radius * sin(theta_ref);

        part.is_line = 1;
        part.center_x = 0;
        part.center_y = 0;
        part.direction = 0;
        part.slope = (part.y2 - part.y1) / (part.x2 - part.x1);
        part.y_intercept = part.y1 - part.slope * part.x1;
        part.theta = atan2(part.y2 - part.y1, part.x2 - part.x1);

        Poly_out.push_back(part);
        count -= 1;
    }

    if (Poly_out.at(Poly_out.size() - 1).x2 != Arc.x2 && Poly_out.at(Poly_out.size() - 1).y2 != Arc.y2)
    {
        part.x1 = Poly_out.at(Poly_out.size() - 1).x2;
        part.y1 = Poly_out.at(Poly_out.size() - 1).y2;
        part.x2 = Arc.x2;
        part.y2 = Arc.y2;

        part.center_x = 0;
        part.center_y = 0;
        part.direction = 0;
        part.slope = (part.y2 - part.y1) / (part.x2 - part.x1);
        part.y_intercept = part.y1 - part.slope * part.x1;
        part.theta = atan2(part.y2 - part.y1, part.x2 - part.x1);

        Poly_out.push_back(part);
    }

    return Poly_out;
}
*/