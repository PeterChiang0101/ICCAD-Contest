// segment.h
// declearation of class segment
// functions for segment is decleared in segment.cpp

#ifndef SEGMENT_H
#define SEGMENT_H

class Segment
{
public:
    Segment();
    void Input(bool, float, float, float, float, float, float, bool);
    void Output();

private:
    bool is_line; // 0 = arc, 1 = line
    float x1;
    float y1;
    float x2;
    float y2;
    float slope;       //斜率
    float y_intercept; //截距
    // below is needed by arc, when deals with line set all to 0
    float center_x; //圓心x
    float center_y; //圓心y
    bool direction; // 0 = ClockWise(CW), 1 = ConterClockwise(CCW)
};

#endif
