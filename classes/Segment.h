// Segment.h
// declearation of struct Segment

#include "Detail.h"

#ifndef SEGMENT_H
#define SEGMENT_H

struct Segment
{
    bool is_line; // 0 = arc, 1 = line
    float x1;
    float y1;
    float x2;
    float y2;

    // below are only used when is_line = 0
    float center_x;
    float center_y;
    bool is_CCW; // 0 = ClockWise(CW), 1 = ConterClockwise(CCW)

    Detail detail;
};

#endif // SEGMENT_H
