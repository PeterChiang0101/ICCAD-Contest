#include <bits/stdc++.h>

#include "./classes/FileIO.h"
#include "./classes/Buffer.h"
#include "./classes/Silkscreen.h"

#include "./classes/Graph.h"
using namespace std;

int main()
{

    for(int i = 0; i < 179; i++)
    {
        if(i == 80) continue;
        float assemblygap = 0.0;
        float coppergap = 0.0;
        float silkscreenlen = 0.0;
        FileIO File;
        string filename;
        string filename_2;
        filename = "D:/iccad_contest/ICCAD-Contest/test_case/testcase_" + to_string(i+1) + ".txt";
        File.Read_File(filename);
        assemblygap = File.File_to_Parameter() * 1.0001;
        coppergap = File.File_to_Parameter() * 1.0001;
        silkscreenlen = File.File_to_Parameter() * 1.0001;
        Graph assembly = File.Read_Assembly();
        vector<Graph> copper = File.Read_Copper();

        Buffer buffer(assemblygap, coppergap);
        Graph assembly_buffer = buffer.Assembly_Buffer(assembly);
        vector<Graph> copper_buffer = buffer.Copper_Buffer(copper);

        Silkscreen silkscreen(silkscreenlen);
        vector<Graph> silkscreen_buffer = silkscreen.Silkscreen_Assembly(assembly, assembly_buffer, copper_buffer);
        filename_2 = "D:/iccad_contest/ICCAD-Contest/test_case/testcase_" + to_string(i+1) + "_Ans.txt";
        File.Write_File(silkscreen_buffer, filename_2);
    }
    
    return 0;
}