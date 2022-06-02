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
#define INPUT_PATH "./TestingCase/test_C.txt"
#define OUTPUT_PATH "./TestingCase/test_C_Ans.txt"
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
    double theta_1; // 圓心到點一角度
    double theta_2; // 圓心到點二角度
};

struct Point
{
    float x;
    float y;
    bool Next_Arc; // if the point connected to arc
};

float File_to_Parameter(const string);

const vector<string> split(const string &, const char &);

segment String_to_Line(const string);

vector<segment> Read_Assembly(fstream &);

vector<vector<segment>> Read_Copper(fstream &);

vector<Point> Line_to_Point(const vector<segment>); //將線段切割成點

vector<segment> Buffer(const vector<segment>);

float interpolate_x(const float, const Point, const Point);

bool point_in_polygon(const Point, const vector<Point>, const vector<vector<Point>>);

// bool Outside_of_Assembly(const Point, const vector<segment>);

void Write_File(const vector<segment>);

vector<segment> Arc_to_Poly(segment);

vector<vector<Point>> Arc_Optimization(const vector<segment>);

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

    silkscreen = Buffer(assembly);

    Write_File(silkscreen);
    vector<segment> copper_barrier;
    vector<vector<segment>> whole_copper_barrier;

    for (int i = 0; i < copper.size(); i++)
    {
        copper_barrier.clear();
        copper_barrier = Buffer(copper[i]);
        Write_File(copper_barrier);
        whole_copper_barrier.push_back(copper_barrier);
    }
    // calculate the silkscreen
    // ignore the arc first

    // arc needed to be treated manually

    // output
}

float File_to_Parameter(const string str) // 讀入參數
{
    string str_truncate;
    str_truncate = str.substr(str.find(',') + 1);
    return stof(str_truncate);
}

const vector<string> split(const string &str, const char &delimiter) // 拆分文字
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

segment String_to_Line(string line) // 讀取時建立線段
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

vector<segment> Read_Assembly(fstream &Input_File) // 讀取assembly，轉換為vector
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

vector<vector<segment>> Read_Copper(fstream &Input_File) // 讀取copper，轉換為二維vector
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

vector<Point> Line_to_Point(const vector<segment> Assembly) // 將線段切割成點
{
    const int size = Assembly.size();
    vector<Point> Point_Vector;
    segment first_line, second_line;
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

vector<segment> Buffer(const vector<segment> Assembly) // 圖形外擴
{
    const int size = Assembly.size();
    vector<Point> Assembly_Points;
    vector<Point> Extended_Points;
    vector<vector<Point>> Arc_Dots;
    segment A_Line;
    vector<segment> Silkscreen;

    Assembly_Points = Line_to_Point(Assembly); //線切割為點
    Arc_Dots = Arc_Optimization(Assembly);     // 將圓弧切割成多個點，以利辨識點在圖形內外

    for (size_t i = 0; i < size; i++)
    {
        segment first_line, second_line;
        double first_angle, second_angle;
        first_line = Assembly[i];
        if (i != size - 1)
            second_line = Assembly[i + 1];
        else
            second_line = Assembly[0];

        if (first_line.is_line) // first segment is line
            first_angle = first_line.theta;
        else // first segment is arc
            first_angle = (first_line.direction) ? first_line.theta_2 + PI / 2 : first_line.theta_2 - PI / 2;

        if (second_line.is_line) // line
            second_angle = second_line.theta;
        else // arc direction 0 = ClockWise(CW), 1 = ConterClockwise(CCW)
            second_angle = (second_line.direction) ? second_line.theta_1 + PI / 2 : second_line.theta_1 - PI / 2;

        if (Assembly_Points[i].x == first_line.x1 && Assembly_Points[i].y == first_line.y1) // 向量共同點校正
        {
            first_angle -= PI;
            if (first_angle < -PI)
                first_angle += 2 * PI;
        }
        if (Assembly_Points[i].x == second_line.x1 && Assembly_Points[i].y == second_line.y1)
        {
            second_angle -= PI;
            if (second_angle < -PI)
                second_angle += 2 * PI;
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

        Extend_1.Next_Arc = Assembly_Points[i].Next_Arc;
        Extend_2.Next_Arc = Assembly_Points[i].Next_Arc;

        //點是否在圖型外
        Outside_1 = !point_in_polygon(Extend_1, Assembly_Points, Arc_Dots); // true for outside, false for inside
        Outside_2 = !point_in_polygon(Extend_2, Assembly_Points, Arc_Dots);

        if (Outside_1 && !Outside_2) // 1 is outside, 2 is inside
            Extended_Points.push_back(Extend_1);
        else if (Outside_2 && !Outside_1) // 2 is outside, 1 is inside
            Extended_Points.push_back(Extend_2);
    }
    for (size_t i = 0; i < size; i++) // for line
    {
        A_Line.is_line = (Extended_Points[i].Next_Arc) ? false : true;
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
        if (A_Line.is_line)
        {
            A_Line.slope = (A_Line.y2 - A_Line.y1) / (A_Line.x2 - A_Line.x1);
            A_Line.y_intercept = A_Line.y1 - A_Line.slope * A_Line.x1;
            A_Line.center_x = A_Line.center_y = A_Line.direction = 0;
        }
        else
        {
            A_Line.slope = A_Line.y_intercept = A_Line.theta = 0;
            if (i != size - 1)
            {
                A_Line.center_x = Assembly[i + 1].center_x;
                A_Line.center_y = Assembly[i + 1].center_y;
                A_Line.direction = Assembly[i + 1].direction;
            }
            else
            {
                A_Line.center_x = Assembly[0].center_x;
                A_Line.center_y = Assembly[0].center_y;
                A_Line.direction = Assembly[0].direction;
            }
            A_Line.theta_1 = atan2(A_Line.y1 - A_Line.center_y, A_Line.x1 - A_Line.center_x);
            A_Line.theta_2 = atan2(A_Line.y2 - A_Line.center_y, A_Line.x2 - A_Line.center_x);
        }
        Silkscreen.push_back(A_Line);
    }
    return Silkscreen;
}

float interpolate_x(float y, Point p1, Point p2) // 待測點與圖形邊界交會的x值
{
    if (p1.y == p2.y)
        return p1.x;
    return p1.x + (p2.x - p1.x) * (y - p1.y) / (p2.y - p1.y);
}

bool point_in_polygon(Point t, vector<Point> Assembly_Point, vector<vector<Point>> Arc_Points) // 運用射線法判斷點在圖形內外
{
    int Assembly_size = Assembly_Point.size();
    int Arc_count = 0;
    bool c = false;
    for (int i = Assembly_size - 1, j = 0; j < Assembly_size; i = j++)
    {
        if (Assembly_Point[i].Next_Arc)
        {
            int Arc_point_length = Arc_Points[Arc_count].size();
            for (int k = Arc_point_length - 1, l = 0; l < Arc_point_length; k = l++)
            {
                if ((Arc_Points[Arc_count][k].y > t.y) != (Arc_Points[Arc_count][l].y > t.y) && t.x < interpolate_x(t.y, Arc_Points[Arc_count][k], Arc_Points[Arc_count][l]))
                    c = !c;
            }
        }
        else
            // 待測點在該線段的高度上下限內 且 交會點x值大於待測點x值 (射線為 + x 方向)
            if ((Assembly_Point[i].y > t.y) != (Assembly_Point[j].y > t.y) && t.x < interpolate_x(t.y, Assembly_Point[i], Assembly_Point[j]))
                c = !c;
    }
    return c;
}

void Write_File(const vector<segment> Silkscreen)
{
    fstream Output;

    Output.open(OUTPUT_PATH, ios::app);
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
            Output << "arc," << fixed << setprecision(4) << Silkscreen[i].x1 << "," << Silkscreen[i].y1 << "," << Silkscreen[i].x2 << "," << Silkscreen[i].y2 << "," << Silkscreen[i].center_x << "," << Silkscreen[i].center_y << "," << (Silkscreen[i].direction ? "CCW" : "CW") << endl;
        }
    }
}

vector<segment> Arc_to_Poly(segment Arc)
{
    vector<segment> Poly_out;
    segment part;

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
            part.x1 = Poly_out[times - count - 1].x2;
            part.y1 = Poly_out[times - count - 1].y2;
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

    if (Poly_out[Poly_out.size() - 1].x2 != Arc.x2 && Poly_out[Poly_out.size() - 1].y2 != Arc.y2)
    {
        part.x1 = Poly_out[Poly_out.size() - 1].x2;
        part.y1 = Poly_out[Poly_out.size() - 1].y2;
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

vector<vector<Point>> Arc_Optimization(vector<segment> Assembly)
{
    int Assembly_size = Assembly.size();
    vector<Point> Dots_of_Arc;
    vector<vector<Point>> vector_of_Arc;
    for (int i = 0; i < Assembly_size; i++)
    {
        if (!Assembly[i].is_line) // the segment is arc
        {
            Dots_of_Arc.clear();
            Dots_of_Arc = Line_to_Point(Arc_to_Poly(Assembly[i]));
            vector_of_Arc.push_back(Dots_of_Arc);
        }
    }
    return vector_of_Arc;
}
///////////////////////////////////////////////////////
///////////////// abandoned functions /////////////////
///////////////////////////////////////////////////////

/*
// 原因：原本想用線段外擴求焦點，因過於複雜放棄，改用點外擴 Buffer()

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

/*
vector<Point> Arc_to_Line(const vector<segment> Assembly)
{
    const int size = Assembly.size();
    for (size_t i = 0; i < size; i++)
    {
        segment new_line;
        segment first_line, second_line;
        first_line = Assembly[i];
        if (i != size - 1)
            second_line = Assembly[i + 1];
        else
            second_line = Assembly[0];
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
    //the struct "segment" need a extra bool to tell Silkscreen_Buffer the first and second line are forbidden to extrapolate
}
*/

/*
// 原因：此方法僅適用凸多邊形，Case C 會導致錯誤，改用射線法 point_in_polygon()

bool Outside_of_Assembly(const Point a, const vector<segment> Assembly) //  使用角度相加為2 * PI，
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
*/