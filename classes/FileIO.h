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
    void Read_File(const char *filename);
    void Read_File(const string filename);
    void Read_File(const char *filename, const char *filename_ans);
    float File_to_Parameter(); // 讀入參數
    Graph Read_Assembly();
    vector<Graph> Read_Copper();
    Graph Read_Silkscreen();
    void Write_File(const Graph);                               // 印出未切割的絲
    void Write_File(const vector<Graph>, const char *filename); // 可印出切割後的絲印和原始和外擴的copper
    void Write_File(const vector<Graph>, const string);

private:
    fstream InFile;
    fstream InFileAns;
    vector<int> continue_num; // 每個連續線段的線段數量
    Segment String_to_Line(string);                     // 讀取時建立線段
    vector<string> split(const string &, const char &); // 拆分文字
    Segment Arc_Boundary_Meas_for_Assembly(Segment);    // 計算圓弧邊界
};

#endif // FILEIO_H