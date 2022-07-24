// Detail.h
// segment's detail after calculation

#ifndef DETAIL_H
#define DETAIL_H

struct Detail
{
    float x_min, x_max, y_min, y_max; // the bounding box of the segment

    // below are only used when is_line = 1
    double theta; // the angle reference to positive x axis

    // below are only used when is_line = 0
    double theta_1; // 圓心到點一角度
    double theta_2; // 圓心到點二角度
    float radius;   // 半徑
};

#endif