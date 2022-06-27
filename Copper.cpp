#include <bits/stdc++.h>
#include "Segment.h"
#include "Copper.h"

using namespace std;

Copper::Copper()
{
}

vector<vector<Segment>> Copper::Read_Copper(fstream &Input_File)
{
    Segment part;
    vector<string> split_return;
    string line;
    while (getline(Input_File, line))
    {
        if (line == "copper")
        {
            copper_pack.push_back(segment);
            segment.clear();
        }
        else
        {
            part.String_to_Line(line);
            segment.push_back(part);
        }
    }
    copper_pack.push_back(segment);
    return copper_pack;
}

vector<Copper> Copper_Buffer(const vector<vector<Segment>> coppers)
{
    int size = coppers.size();
    Copper Single_Copper;
    vector<Copper> Every_Copper;
    for (int i = 0; i < size; i++)
    {
        Single_Copper = Copper_Point_to_Line(Point_Extension(coppers.at(i), false), coppers.at(i));
        Every_Copper.push_back(Single_Copper);
    }
    return Every_Copper;
}

Copper Copper_Point_to_Line(vector<Point> Extended_Points, vector<Segment> copper)
{
    int size = copper.size();
    Segment A_Line;
    Copper Silkscreen, Arc_Boundary;
    if (!Extended_Points.empty())
    {
        Silkscreen.x_min = Silkscreen.x_max = Extended_Points.at(0).x; // initialize
        Silkscreen.y_min = Silkscreen.y_max = Extended_Points.at(0).y;
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
