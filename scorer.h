#include <bits/stdc++.h>
#include "inputoutput.h"
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
    Scorer(const char*,const char*);
    void open_file();
    double first_quarter();//const vector<Segment>, const vector<Segment>);
    double second_quarter();//const vector<Segment>, const vector<Segment>);
    double third_quarter();//const vector<vector<Segment>>, const vector<Segment>);
    double fourth_quarter();//const vector<Segment>, const vector<Segment>);
    Scorer& setAssembly(const vector<Segment>);
    Scorer& setCopper(const vector<vector<Segment>>);
    Scorer& setSilkscreen(const vector<Segment>);

private:
    Segment S1;
    fstream Q_file; // 題目
    fstream A_file; // 答案
    float assemblygap, coppergap, silkscreenlen;
    vector<Segment> assembly;
    vector<vector<Segment>> copper;
    vector<Segment> silkscreen;
    vector<Segment> Assembly_push_out;
    vector<Segment> Read_Silkscreen(fstream &);
};

#endif