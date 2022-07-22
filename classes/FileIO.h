// FileIO.h
// declearation of input/output functions

#include <bits/stdc++.h>
#include "Segment.h"
#include "Copper.h"

using namespace std;

#ifndef FILEIO_H
#define FILEIO_H

class FileIO
{
public:
    float File_to_Parameter(const string str); // 讀入參數

    vector<Segment> Read_Assembly(fstream &);

    vector<vector<Segment>> Read_Copper(fstream &);

    void Write_File(const vector<Segment>);

    void Write_File(const vector<vector<Segment>>, char **argv);

    void Write_File_Copper(const vector<Copper>); // debugging function

private:
};

#endif // FILEIO_H