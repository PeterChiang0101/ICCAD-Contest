// Segment.h
// declearation of struct Segment

#ifndef SEGMENT_H
#define SEGMENT_H

struct Segment
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
};

#endif // SEGMENT_H
