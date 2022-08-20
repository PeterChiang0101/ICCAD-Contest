#include <bits/stdc++.h>

#include "FileIO.h"
#include "Buffer.h"
#include "Silkscreen.h"

#include "Graph.h"
using namespace std;

int main(int argc, char **argv)
{
    float assemblygap = 0.0;
    float coppergap = 0.0;
    float silkscreenlen = 0.0;
    FileIO File;
    File.Read_File(argv[1]);
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

    File.Write_File(silkscreen_buffer, argv[2]);
    return 0;
}