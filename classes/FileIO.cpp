// FileIO.h
// implementation of input/output functions

#include <bits/stdc++.h>
#include "FileIO.h"

using namespace std;

float FileIO::File_to_Parameter(const string str)
{
    string str_truncate;
    str_truncate = str.substr(str.find(',') + 1);
    return stof(str_truncate);
}