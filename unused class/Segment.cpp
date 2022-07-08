// Segment.cpp
// functions of class Segment

#include <iostream>
#include <cmath>
#include "Segment.h"

using namespace std;

Segment::Segment()
{
    is_line = 0;
    x1 = x2 = y1 = y2 = 0;
    slope = y_intercept = 0;
    center_x = center_y = direction = 0;
}

void Segment::String_to_Line(string line)
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
            x1 = stof(Splited.at(i));
            break;
        case 2:
            y1 = stof(Splited.at(i));
            break;
        case 3:
            x2 = stof(Splited.at(i));
            break;
        case 4:
            y2 = stof(Splited.at(i));
            break;
        case 5:
            center_x = stof(Splited.at(i));
            break;
        case 6:
            center_y = stof(Splited.at(i));
            break;
        case 7:
            direction = (Splited.at(i) == "CCW") ? 1 : 0;
            break;
        }
    }
    if (vector_size == 5)
    {
        is_line = true;
        center_x = 0;
        center_y = 0;
        direction = 0;
        slope = (y2 - y1) / (x2 - x1);
        y_intercept = y1 - slope * x1;
        theta = atan2(y2 - y1, x2 - x1);
        // theta_1 = 0 , theta_2 = 0 ???
    }
    else if (vector_size == 8)
    {
        is_line = false;
        slope = 0;
        y_intercept = 0;
        theta = 0;
        theta_1 = atan2(y1 - center_y, x1 - center_x);
        theta_2 = atan2(y2 - center_y, x2 - center_x);
    }
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