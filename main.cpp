#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring> // strtok
#include <vector>
#include <cmath>
// use boost library

using namespace std;
float assemblygap, coppergap, silkscreenlen;
// assemblygap : the minimum distance between assembly and silkscreen
// coppergap : the minimum distance between copper and silkscreen
// silkscreenlen : the minimum length of silkscreen
struct segment
{
    bool is_line; // 0 = arc, 1 = line
    float x1;
    float y1;
    float x2;
    float y2;
    float slope;       //斜率
    float y_intercept; //截距
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

const vector<string>
split(const string &str, const char &delimiter);

float File_to_String(string str);

vector<segment> Print_Silkscreen(vector<segment> Assembly);

int main()
{
    fstream file;
    string assemblygap_str, coppergap_str, silkscreenlen_str;

    file.open("test.txt", ios::in);

    // the first three line of the file, defines parameters for silkscreen
    file >> assemblygap_str >> coppergap_str >> silkscreenlen_str;
    assemblygap = File_to_String(assemblygap_str);
    coppergap = File_to_String(coppergap_str);
    silkscreenlen = File_to_String(silkscreenlen_str);

    vector<segment> assembly;
    vector<vector<segment>> copper;
    vector<segment> copper_master;
    vector<string> ret;
    vector<segment> silkscreen;

    struct segment master;
    bool type;       // 0 = assembly, 1 = copper
    int element = 0; // which element of a segment
    int num = 0;     // which copper

    string line;
    getline(file, line); // stange string???????

    while (getline(file, line))
    {
        cout << line << endl;
        if (line == "assembly")
        {
            type = 0;
            continue;
        }
        else if (line == "copper")
        {
            type = 1;
            num++;
            if (num != 1)
            {
                cout << "size0=" << copper_master.size() << endl;
                copper.push_back(copper_master);
                copper_master.clear();
            }
            continue;
        }
        else
        {
            ret = split(line, ',');
            for (auto &s : ret)
            {
                if (s == "line") // reading line
                {
                    master.is_line = 1;
                    master.center_x = 0;
                    master.center_y = 0;
                    master.direction = 0;
                    element = 0;
                }
                else if (s == "arc") // reading arc
                {
                    master.is_line = 0;
                    element = 0;
                }
                else
                {
                    element++;
                    switch (element)
                    {
                    case 1:
                        master.x1 = stof(s);
                        break;
                    case 2:
                        master.y1 = stof(s);
                        break;
                    case 3:
                        master.x2 = stof(s);
                        break;
                    case 4:
                        master.y2 = stof(s);
                        break;
                    case 5:
                        master.center_x = stof(s);
                        break;
                    case 6:
                        master.center_y = stof(s);
                        break;
                    case 7:
                        if (s == "CW")
                            master.direction = 0;
                        else
                            master.direction = 1;
                        break;
                    }
                }
            }
            master.slope = (master.y2 - master.y1) / (master.x2 - master.x1);
            master.y_intercept = master.y1 - master.slope * master.x1;

            if (type == 0)
            {
                assembly.push_back(master);
            }
            else
            {
                // cout<<"num="<<num<<endl;
                copper_master.push_back(master);
            }
        }
    }
    copper.push_back(copper_master);
    copper_master.clear();

    silkscreen = Print_Silkscreen(assembly);
    // the main IC uses polygon
    // arc uses linestring
    // pin uses polygon

    // calculate the silkscreen
    // ignore the arc first
    // maximize the usage of boost
    // buffer() , closest_point(), distance(), within()

    // arc needed to be treated manually

    // output
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

float File_to_String(string str)
{
    string str_truncate;
    str_truncate = str.substr(str.find(',') + 1);
    return stof(str_truncate);
}

segment line_offset(segment original_line, float assemblygap)
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

vector<segment> Print_Silkscreen(vector<segment> Assembly)
{
    const int size = Assembly.size();
    for (size_t i = 0; i < size - 1; i++)
    {
        segment first_line = Assembly[i], second_line = Assembly[i + 1];
        double first_angle = atan2(first_line.slope, 1);
        double second_angle = atan2(second_line.slope, 1);
        Point Point_Overlap; //兩線段交點
        if (first_line.x1 == second_line.x1 || first_line.x1 == second_line.x2)
        {
            Point_Overlap.x = first_line.x1;
            Point_Overlap.y = first_line.y1;
        }
        else
        {
            Point_Overlap.x = first_line.x2;
            Point_Overlap.y = first_line.y2;
        }
        double Angle_Divided = (first_angle + second_angle) / 2;                    //角平分線的角度
        float Bisector_Slope = tan(Angle_Divided);                                  //角平分線
        double Point_Extend_Range = assemblygap / sin(Angle_Divided - first_angle); //點外擴距離
        Point Extend_1, Extend_2;                                                   //交點向外延伸的兩個點
        Extend_1.x = Point_Overlap.x + Point_Extend_Range * sin(Angle_Divided);
        Extend_1.y = Point_Overlap.y + Point_Extend_Range * cos(Angle_Divided);
        Extend_2.x = Point_Overlap.x - Point_Extend_Range * sin(Angle_Divided);
        Extend_2.y = Point_Overlap.y - Point_Extend_Range * cos(Angle_Divided);
    }
}