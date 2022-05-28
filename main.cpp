#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring> // strtok
#include <vector>
#include <cmath>
#include <iomanip> //setprecision & fixed

using namespace std;
#define Angle_Tolerance 0.1 //算角度誤差容許值 (rad)
#define PI 3.14159265358979323846
#define INPUT_PATH "./TestingCase/test_A.txt"
#define OUTPUT_PATH "./TestingCase/test_A_Ans.txt"
// assemblygap : the minimum distance between assembly and silkscreen
// coppergap : the minimum distance between copper and silkscreen
// silkscreenlen : the minimum length of silkscreen
float assemblygap, coppergap, silkscreenlen;

struct segment
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
};

struct Point
{
    float x;
    float y;
};

float File_to_Parameter(const string);

const vector<string> split(const string &, const char &);

segment String_to_Line(const string);

vector<segment> Read_Assembly(fstream &);

vector<vector<segment>> Read_Copper(fstream &);

vector<Point> Line_to_Point(const vector<segment>); //將線段切割成點

vector<segment> Silkscreen_Buffer(const vector<segment>);

bool Outside_of_Assembly(const Point, const vector<segment>);

void Write_File(const vector<segment>);

int main()
{
    fstream file;
    string assemblygap_str, coppergap_str, silkscreenlen_str;

    file.open(INPUT_PATH, ios::in);

    // the first three line of the file, defines parameters for silkscreen
    file >> assemblygap_str >> coppergap_str >> silkscreenlen_str;
    assemblygap = File_to_Parameter(assemblygap_str);
    coppergap = File_to_Parameter(coppergap_str);
    silkscreenlen = File_to_Parameter(silkscreenlen_str);

    vector<segment> assembly;
    vector<vector<segment>> copper;
    vector<segment> silkscreen;

    assembly = Read_Assembly(file);
    copper = Read_Copper(file);

    silkscreen = Silkscreen_Buffer(assembly);

    Write_File(silkscreen);

    // calculate the silkscreen
    // ignore the arc first

    // arc needed to be treated manually

    // output
}

float File_to_Parameter(const string str)
{
    string str_truncate;
    str_truncate = str.substr(str.find(',') + 1);
    return stof(str_truncate);
}

const vector<string> split(const string &str, const char &delimiter)
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

segment String_to_Line(string line)
{
    vector<string> Splited;
    Splited = split(line, ',');
    int vector_size = Splited.size();
    segment part;

    for (size_t i = 1; i < vector_size; i++)
    {
        switch (i)
        {
        case 1:
            part.x1 = stof(Splited[i]);
            break;
        case 2:
            part.y1 = stof(Splited[i]);
            break;
        case 3:
            part.x2 = stof(Splited[i]);
            break;
        case 4:
            part.y2 = stof(Splited[i]);
            break;
        case 5:
            part.center_x = stof(Splited[i]);
            break;
        case 6:
            part.center_y = stof(Splited[i]);
            break;
        case 7:
            part.direction = (Splited[i] == "CCW") ? 1 : 0;
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
    }
    else if (vector_size == 8)
    {
        part.is_line = false;
        part.slope = 0;
        part.y_intercept = 0;
        part.theta = 0;
    }
    return part;
}

vector<segment> Read_Assembly(fstream &Input_File)
{
    vector<segment> Assembly;
    segment part;
    vector<string> split_return;
    string line;
    int line_size;
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

vector<vector<segment>> Read_Copper(fstream &Input_File)
{
    vector<segment> copper;
    vector<vector<segment>> copper_pack;
    segment part;
    vector<string> split_return;
    string line;
    int line_size;
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

/*
segment line_offset(const segment original_line, const float assemblygap) // not implemented
{
    float line_length;
    float x_offset, y_offset;
    segment silkscreen;
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

vector<Point> Line_to_Point(const vector<segment> Assembly) //將線段切割成點
{
    const int size = Assembly.size();
    vector<Point> Point_Vector;
    segment first_line, second_line;
    double first_angle, second_angle;
    Point Point_Overlap; //兩線段交點

    for (size_t i = 0; i < size; i++)
    {
        first_line = Assembly[i];
        if (i != size - 1) // the line is not the last line
            second_line = Assembly[i + 1];
        else // the line is the last line
            second_line = Assembly[0];
        if ((first_line.x1 == second_line.x1 && first_line.y1 == second_line.y1) || (first_line.x1 == second_line.x2 && first_line.y1 == second_line.y2)) //找重疊線段
        {
            Point_Overlap.x = first_line.x1;
            Point_Overlap.y = first_line.y1;
        }
        else
        {
            Point_Overlap.x = first_line.x2;
            Point_Overlap.y = first_line.y2;
        }
        Point_Vector.push_back(Point_Overlap);
    }
    return Point_Vector;
}

vector<segment> Silkscreen_Buffer(const vector<segment> Assembly) //產生絲印
{
    const int size = Assembly.size();
    vector<Point> Assembly_Points;
    vector<Point> Extended_Points;
    segment A_Line;
    vector<segment> Silkscreen;

    Assembly_Points = Line_to_Point(Assembly); //線切割為點

    for (size_t i = 0; i < size; i++)
    {
        segment first_line, second_line;
        first_line = Assembly[i];
        if (i != size - 1)
            second_line = Assembly[i + 1];
        else
            second_line = Assembly[0];
        double first_angle = atan2(first_line.y2 - first_line.y1, first_line.x2 - first_line.x1); //不可用斜率
        double second_angle = atan2(second_line.y2 - second_line.y1, second_line.x2 - second_line.x1);
        if (Assembly_Points[i].x == first_line.x1 && Assembly_Points[i].y == first_line.y1) // 向量共同點校正
        {
            first_angle -= PI;
            if (first_angle < -PI)
                first_angle += 2 * PI;
        }
        if (Assembly_Points[i].x == second_line.x1 && Assembly_Points[i].y == second_line.y1)
        {
            second_angle -= PI;
            if (first_angle < -PI)
                first_angle += 2 * PI;
        }

        double Angle_Divided = (first_angle + second_angle) / 2;                    //角平分線的角度
        float Bisector_Slope = tan(Angle_Divided);                                  //角平分線
        double Point_Extend_Range = assemblygap / sin(Angle_Divided - first_angle); //點外擴距離
        Point Extend_1, Extend_2;                                                   //交點向外延伸的兩個點
        bool Outside_1, Outside_2;
        Extend_1.x = Assembly_Points[i].x + Point_Extend_Range * cos(Angle_Divided);
        Extend_1.y = Assembly_Points[i].y + Point_Extend_Range * sin(Angle_Divided);
        Extend_2.x = Assembly_Points[i].x - Point_Extend_Range * cos(Angle_Divided);
        Extend_2.y = Assembly_Points[i].y - Point_Extend_Range * sin(Angle_Divided);

        //點是否在圖型外
        Outside_1 = Outside_of_Assembly(Extend_1, Assembly); // true for outside, false for inside
        Outside_2 = Outside_of_Assembly(Extend_2, Assembly);

        if (Outside_1 && !Outside_2) // 1 is outside, 2 is inside
            Extended_Points.push_back(Extend_1);
        else if (Outside_2 && !Outside_1) // 2 is outside, 1 is inside
            Extended_Points.push_back(Extend_2);
    }
    for (size_t i = 0; i < size; i++) // for line
    {
        A_Line.is_line = true;
        A_Line.x1 = Extended_Points[i].x;
        A_Line.y1 = Extended_Points[i].y;
        if (i != size - 1)
        {
            A_Line.x2 = Extended_Points[i + 1].x;
            A_Line.y2 = Extended_Points[i + 1].y;
        }
        else
        {
            A_Line.x2 = Extended_Points[0].x;
            A_Line.y2 = Extended_Points[0].y;
        }
        A_Line.slope = (A_Line.y2 - A_Line.y1) / (A_Line.x2 - A_Line.x1);
        A_Line.y_intercept = A_Line.y1 - A_Line.slope * A_Line.x1;
        A_Line.center_x = A_Line.center_y = A_Line.direction = 0;
        Silkscreen.push_back(A_Line);
    }
    return Silkscreen;
}

bool Outside_of_Assembly(const Point a, const vector<segment> Assembly) //使用角度方法
{
    vector<Point> Assembly_Points = Line_to_Point(Assembly);
    vector<float> Angle_Vector; // 各點之間角度差
    float Angle;
    float First_Angle, Second_Angle;
    float Total_Angle = 0;

    int size = Assembly_Points.size();
    for (size_t i = 0; i < size; i++)
    {
        Angle_Vector.push_back(atan2(Assembly_Points[i].y - a.y, Assembly_Points[i].x - a.x));
    }
    for (size_t i = 0; i < size; i++)
    {
        First_Angle = Angle_Vector[i];
        if (i == size - 1)
            Second_Angle = Angle_Vector[0];
        else
            Second_Angle = Angle_Vector[i + 1];
        Angle = abs(Second_Angle - First_Angle);       // 角度差
        Angle = (Angle > PI) ? 2 * PI - Angle : Angle; // 差值永遠為正
        Total_Angle += Angle;
    }

    if (abs(Total_Angle - 2 * PI) > Angle_Tolerance)
        return true;
    else
        return false;
}

void Write_File(const vector<segment> Silkscreen)
{
    fstream Output;

    Output.open(OUTPUT_PATH, ios::out);
    Output << "silkscreen" << endl;
    const int size = Silkscreen.size();
    for (size_t i = 0; i < size; i++)
    {
        if (Silkscreen[i].is_line)
        {
            Output << "line," << fixed << setprecision(4) << Silkscreen[i].x1 << "," << Silkscreen[i].y1 << "," << Silkscreen[i].x2 << "," << Silkscreen[i].y2 << endl;
        }
        else
        {
            Output << "Arc," << fixed << setprecision(4) << Silkscreen[i].x1 << "," << Silkscreen[i].y1 << "," << Silkscreen[i].x2 << "," << Silkscreen[i].y2 << "," << Silkscreen[i].center_x << "," << Silkscreen[i].center_y << (Silkscreen[i].direction ? "CCW" : "CW") << endl;
        }
    }
}