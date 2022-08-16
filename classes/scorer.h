#include <bits/stdc++.h>
#include "inputoutput.h"
#include "FileIO.h"
#include "Buffer.h"

using namespace std;

#ifndef SCORER_H
#define SCORER_H

/*struct Segment
{
    bool is_line; // 0 = arc, 1 = line
    float x1;
    float y1;
    float x2;
    float y2;
    float x_min, x_max, y_min, y_max; // the bounding box of the segment

    // below are only used when is_line = 1
    float slope;       //斜率
    float y_intercept; //截距
    double theta;      // the angle reference to positive x axis

    // below are only used when is_line = 0
    float center_x;
    float center_y;
    bool direction; // 0 = ClockWise(CW), 1 = ConterClockwise(CCW)
    double theta_1; // 圓心到點一角度
    double theta_2; // 圓心到點二角度
};*/

class Scorer
{
public:
    Scorer();
    Scorer(const char *, const char *);
    void read_file();
    double first_quarter();  
    double second_quarter(); 
    double third_quarter();  
    double fourth_quarter(); 
    double Total_score(bool);
    Scorer &setAssembly(const Graph);
    Scorer &setCopper(const vector<Graph>);
    Scorer &setSilkscreen(const Graph);

private:
    FileIO file;
    double Arc_Degree(const Segment &S1);
    
    Segment S1;
    //fstream Q_file; // 題目
    //fstream A_file; // 答案
    float assemblygap, coppergap, silkscreenlen;
    vector<int> continue_num; // 每個連續線段的線段數量
    Graph assembly;
    vector<Graph> copper;
    Graph silkscreen;
    Graph Assembly_push_out;
    bool ShowDetail = false;
};

#endif