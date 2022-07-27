// FileIO.h
// declearation of input/output functions

#include <bits/stdc++.h>
#include "Segment.h"
#include "Graph.h"

using namespace std;

#ifndef FILEIO_H
#define FILEIO_H

class FileIO
{
public:
    float File_to_Parameter(const string str); // 讀入參數
    Graph Read_Assembly(fstream &);
    vector<Graph> Read_Copper(fstream &);
    void Write_File(const Graph);                      // 印出未切割的絲
    void Write_File(const vector<Graph>, char **argv); // 可印出切割後的絲印和原始和外擴的copper

private:
    Segment String_to_Line(string);                     // 讀取時建立線段
    vector<string> split(const string &, const char &); // 拆分文字
    Segment Arc_Boundary_Meas_for_Assembly(Segment);    // 計算圓弧邊界
};

#endif // FILEIO_H