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

void Segment::Input(bool line, float spot1_x, float spot1_y, float spot2_x, float spot2_y, float circle_x = 0, float circle_y = 0, bool dir = 0)
{
    is_line = line;
    x1 = spot1_x;
    y1 = spot1_y;
    x2 = spot2_x;
    y2 = spot2_y;

    if (is_line) // line
    {
        slope = (y2 - y1) / (x2 - x1);
        y_intercept = y1 - slope * x1;
    }
    else // arc
    {
        center_x = circle_x;
        center_y = circle_y;
        direction = dir;
    }
}
