// Detail.h
// segment's detail after calculation

#ifndef DETAIL_H
#define DETAIL_H

struct Detail
{
    float x_min{-5}, x_max{-5}, y_min{-5}, y_max{-5}; // the bounding box of the segment
    size_t SegmentID{0}; //the index of the segment, used to record the intersection segment.
    size_t CopperID{0}; // the index of the copper, used to record the intersection copper.

    // below are only used when is_line = 1
    double theta; // the angle reference to positive x axis

    // below are only used when is_line = 0
    double theta_1; // 圓心到點一角度
    double theta_2; // 圓心到點二角度
    float radius;   // 半徑
};

#endif