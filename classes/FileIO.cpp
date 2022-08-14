// FileIO.h
// implementation of input/output functions

#include <bits/stdc++.h>
#include "FileIO.h"
#include "Segment.h"
#include "Graph.h"
#include "Parameter.h"

using namespace std;

void FileIO::Read_File(const char *filename)
{
    InFile.open(filename, ios::in);
    if (!InFile.is_open())
    {
        cout << "File open failed!" << endl;
        InFile.open(INPUT_PATH, ios::in); // 預設讀取測試檔案
    }
}

float FileIO::File_to_Parameter()
{
    string parameter_str;
    InFile >> parameter_str;
    string str_truncate;
    str_truncate = parameter_str.substr(parameter_str.find(',') + 1);
    return stof(str_truncate);
}

Graph FileIO::Read_Assembly() // 讀取assembly，轉換為vector
{
    Graph Assembly;
    Segment part;
    vector<string> split_return;
    string line;

    Assembly.x_min = INFINITY;
    Assembly.x_max = -INFINITY;
    Assembly.y_min = INFINITY;
    Assembly.y_max = -INFINITY;
    Assembly.segment.clear();

    getline(InFile, line);

    while (getline(InFile, line))
    {
        if (line.find("copper") != string::npos)
            break;
        else if (line.find("assembly") != string::npos)
            continue;
        else
        {
            part = String_to_Line(line);
            if (part.is_line)
            {
                part.detail.theta = atan2(part.y2 - part.y1, part.x2 - part.x1);
                part.detail.theta_1 = 0;
                part.detail.theta_2 = 0;
                part.detail.x_min = min(part.x1, part.x2);
                part.detail.x_max = max(part.x1, part.x2);
                part.detail.y_min = min(part.y1, part.y2);
                part.detail.y_max = max(part.y1, part.y2);
            }
            else
            {
                part.detail.theta = 0;
                part.detail.theta_1 = atan2(part.y1 - part.center_y, part.x1 - part.center_x);
                part.detail.theta_2 = atan2(part.y2 - part.center_y, part.x2 - part.center_x);
                part.detail.radius = hypot(part.x1 - part.center_x, part.y1 - part.center_y);
                part = Arc_Boundary_Meas_for_Assembly(part);
            }
            if (part.detail.x_min < Assembly.x_min)
                Assembly.x_min = part.detail.x_min;
            if (part.detail.x_max > Assembly.x_max)
                Assembly.x_max = part.detail.x_max;
            if (part.detail.y_min < Assembly.y_min)
                Assembly.y_min = part.detail.y_min;
            if (part.detail.y_max > Assembly.y_max)
                Assembly.y_max = part.detail.y_max;
        }

        Assembly.segment.push_back(part);
    }
    return Assembly;
}

vector<Graph> FileIO::Read_Copper() // 讀取copper，轉換為二維vector
{
    Graph copper;
    vector<Graph> copper_pack;
    Segment part;
    vector<string> split_return;
    string line;

    copper.x_min = INFINITY;
    copper.x_max = -INFINITY;
    copper.y_min = INFINITY;
    copper.y_max = -INFINITY;
    copper.segment.clear();
    while (getline(InFile, line))
    {
        if (line.find("copper") != string::npos)
        {
            copper_pack.push_back(copper);
            copper.segment.clear();
        }
        else
        {
            part = String_to_Line(line);
            if (part.is_line)
            {
                part.detail.theta = atan2(part.y2 - part.y1, part.x2 - part.x1);
                part.detail.theta_1 = 0;
                part.detail.theta_2 = 0;
                part.detail.x_min = min(part.x1, part.x2);
                part.detail.x_max = max(part.x1, part.x2);
                part.detail.y_min = min(part.y1, part.y2);
                part.detail.y_max = max(part.y1, part.y2);
            }
            else
            {
                part.detail.theta = 0;
                part.detail.theta_1 = atan2(part.y1 - part.center_y, part.x1 - part.center_x);
                part.detail.theta_2 = atan2(part.y2 - part.center_y, part.x2 - part.center_x);
                part.detail.radius = hypot(part.x1 - part.center_x, part.y1 - part.center_y);
                part = Arc_Boundary_Meas_for_Assembly(part);
            }
            if (part.detail.x_min < copper.x_min)
                copper.x_min = part.detail.x_min;
            if (part.detail.x_max > copper.x_max)
                copper.x_max = part.detail.x_max;
            if (part.detail.y_min < copper.y_min)
                copper.y_min = part.detail.y_min;
            if (part.detail.y_max > copper.y_max)
                copper.y_max = part.detail.y_max;
            copper.segment.push_back(part);
        }
    }
    copper_pack.push_back(copper);
    return copper_pack;
}

void FileIO::Write_File(const Graph Silkscreen)
{
    fstream Output;

    Output.open(OUTPUT_PATH, ios::out);
    // Output << "silkscreen" << endl;
    const size_t size = Silkscreen.segment.size();
    for (size_t i = 0; i < size; i++)
    {
        if (i == 0 || Silkscreen.segment.at(i).x1 != Silkscreen.segment.at(i - 1).x2 || Silkscreen.segment.at(i).y1 != Silkscreen.segment.at(i - 1).y2) // 第一條線，或線段不連續
            Output << "silkscreen" << endl;

        if (Silkscreen.segment.at(i).is_line)
        {
            Output << "line," << fixed << setprecision(4) << Silkscreen.segment.at(i).x1 << "," << Silkscreen.segment.at(i).y1 << "," << Silkscreen.segment.at(i).x2 << "," << Silkscreen.segment.at(i).y2 << endl;
        }
        else
        {
            Output << "arc," << fixed << setprecision(4) << Silkscreen.segment.at(i).x1 << "," << Silkscreen.segment.at(i).y1 << "," << Silkscreen.segment.at(i).x2 << "," << Silkscreen.segment.at(i).y2 << "," << Silkscreen.segment.at(i).center_x << "," << Silkscreen.segment.at(i).center_y << "," << (Silkscreen.segment.at(i).is_CCW ? "CCW" : "CW") << endl;
        }
    }
}

void FileIO::Write_File(const vector<Graph> Silkscreen, const char *filename)
{
    fstream Output;

    Output.open(filename, ios::out);

    if (!Output)
    {
        cout << "Error: Cannot open file" << endl;
        Output.open(OUTPUT_PATH, ios::out); // 如果未指定路徑，使用預設路徑
    }

    const size_t size = Silkscreen.size();
    for (size_t i = 0; i < size; i++)
    {
        int conti_size = Silkscreen.at(i).segment.size();
        if (conti_size == 0)
            continue;
        Output << "silkscreen" << endl;
        for (int j = 0; j < conti_size; j++)
        {
            if (j > 0 && (Silkscreen.at(i).segment.at(j).x1 != Silkscreen.at(i).segment.at(j - 1).x2 || Silkscreen.at(i).segment.at(j).y1 != Silkscreen.at(i).segment.at(j - 1).y2))
                Output << "silkscreen" << endl;
            if (Silkscreen.at(i).segment.at(j).is_line)
            {
                Output << "line," << fixed << setprecision(4) << Silkscreen.at(i).segment.at(j).x1 << "," << Silkscreen.at(i).segment.at(j).y1 << "," << Silkscreen.at(i).segment.at(j).x2 << "," << Silkscreen.at(i).segment.at(j).y2 << endl;
            }
            else
            {
                Output << "arc," << fixed << setprecision(4) << Silkscreen.at(i).segment.at(j).x1 << "," << Silkscreen.at(i).segment.at(j).y1 << "," << Silkscreen.at(i).segment.at(j).x2 << "," << Silkscreen.at(i).segment.at(j).y2 << "," << Silkscreen.at(i).segment.at(j).center_x << "," << Silkscreen.at(i).segment.at(j).center_y << "," << (Silkscreen.at(i).segment.at(j).is_CCW ? "CCW" : "CW") << endl;
            }
        }
    }
}

Segment FileIO::String_to_Line(string line) // 讀取時建立線段
{
    vector<string> Splited;
    Splited = split(line, ',');
    size_t vector_size = Splited.size();
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
            part.is_CCW = (Splited.at(i).find("CCW") != string::npos) ? 1 : 0;
            break;
        }
    }
    if (vector_size == 5)
    {
        part.is_line = true;
        part.center_x = 0;
        part.center_y = 0;
        part.is_CCW = 0;
        // theta_1 = 0 , theta_2 = 0 ???
    }
    else if (vector_size == 8)
    {
        part.is_line = false;
    }
    return part;
}

vector<string> FileIO::split(const string &str, const char &delimiter) // 拆分文字
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

Segment FileIO::Arc_Boundary_Meas_for_Assembly(Segment Arc)
{
    Segment A_Arc;
    A_Arc = Arc;
    float first_angle, second_angle;
    first_angle = Arc.detail.theta_1;
    second_angle = Arc.detail.theta_2;
    if (first_angle == second_angle)
    {
        A_Arc.detail.x_min = Arc.center_x - Arc.detail.radius;
        A_Arc.detail.x_max = Arc.center_x + Arc.detail.radius;
        A_Arc.detail.y_min = Arc.center_y - Arc.detail.radius;
        A_Arc.detail.y_max = Arc.center_y + Arc.detail.radius;
        return A_Arc;
    }

    if (Arc.is_CCW)
    {
        swap(first_angle, second_angle);
    }
    if (first_angle > second_angle)
    {
        A_Arc.detail.x_max = (first_angle >= 0 && second_angle <= 0) ? Arc.center_x + Arc.detail.radius : max(max(Arc.x1, Arc.x2), Arc.center_x);
        A_Arc.detail.x_min = min(min(Arc.x1, Arc.x2), Arc.center_x);
        A_Arc.detail.y_max = (first_angle >= PI / 2 && second_angle <= PI / 2) ? Arc.center_y + Arc.detail.radius : max(max(Arc.y1, Arc.y2), Arc.center_y);
        A_Arc.detail.y_min = (first_angle >= -PI / 2 && second_angle <= -PI / 2) ? Arc.center_y - Arc.detail.radius : min(min(Arc.y1, Arc.y2), Arc.center_y);
    }
    else if (second_angle < 0) // first < second < 0
    {
        A_Arc.detail.x_max = Arc.center_x + Arc.detail.radius;
        A_Arc.detail.x_min = Arc.center_x - Arc.detail.radius;
        A_Arc.detail.y_max = Arc.center_y + Arc.detail.radius;
        A_Arc.detail.y_min = (first_angle >= -PI / 2 || second_angle <= -PI / 2) ? Arc.center_y - Arc.detail.radius : min(min(Arc.y1, Arc.y2), Arc.center_y);
    }
    else if (first_angle >= 0) // 0 <= first < second
    {
        A_Arc.detail.x_max = Arc.center_x + Arc.detail.radius;
        A_Arc.detail.x_min = Arc.center_x - Arc.detail.radius;
        A_Arc.detail.y_max = (first_angle >= PI / 2 || second_angle <= PI / 2) ? Arc.center_y + Arc.detail.radius : max(max(Arc.y1, Arc.y2), Arc.center_y);
        A_Arc.detail.y_min = Arc.center_y - Arc.detail.radius;
    }
    else // first < 0 < second
    {
        A_Arc.detail.x_max = max(max(Arc.x1, Arc.x2), Arc.center_x);
        A_Arc.detail.x_min = Arc.center_x - Arc.detail.radius;
        A_Arc.detail.y_max = (second_angle <= PI / 2) ? Arc.center_y + Arc.detail.radius : max(max(Arc.y1, Arc.y2), Arc.center_y);
        A_Arc.detail.y_min = (first_angle >= -PI / 2) ? Arc.center_y - Arc.detail.radius : min(min(Arc.y1, Arc.y2), Arc.center_y);
    }

    /*A_Arc.x_max = (first >= 0 && second <= 0) ? Arc.center_x + Arc.detail.radius : max(max(Arc.x1, Arc.x2), Arc.center_x);
    A_Arc.x_min = (first >= PI && second <= PI) ? Arc.center_x - Arc.detail.radius : min(min(Arc.x1, Arc.x2), Arc.center_x);
    A_Arc.y_max = (first >= PI / 2 && second <= PI / 2) ? Arc.center_y + Arc.detail.radius : max(max(Arc.y1, Arc.y2), Arc.center_y);
    A_Arc.y_min = (first >= -PI / 2 && second <= -PI / 2) ? Arc.center_y - Arc.detail.radius : min(min(Arc.y1, Arc.y2), Arc.center_y);*/

    return A_Arc;
}