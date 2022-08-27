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
    double File_to_Parameter(); // 讀入參數
    double File_to_Parameter_1x_multiplier(); // 讀入參數，正常倍率，給評分器使用
    Graph Read_Assembly();
    vector<Graph> Read_Copper();
    Graph Read_Silkscreen();
    void Write_File(const Graph);                               // 印出未切割的絲
    void Write_File(const vector<Graph>, const char *filename); // 可印出切割後的絲印和原始和外擴的copper
    void Write_File(const vector<Graph>, const string);
    size_t getcontinue_num_size() const;
    int getcontinue_num_item (size_t order) const;

private:
    fstream InFile;
    fstream InFileAns;
    vector<int> continue_num; // 每個連續線段的線段數量
    GRAPH graph_op; // graph operator
    Segment String_to_Line(string);                     // 讀取時建立線段
    vector<string> split(const string &, const char &); // 拆分文字
    //Segment Arc_Boundary_Meas_for_Assembly(Segment);    // 計算圓弧邊界 //moved to GRAPH
};

#endif // FILEIO_H