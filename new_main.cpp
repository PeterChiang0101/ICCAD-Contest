#include <bits/stdc++.h>
#include "./classes/Parameter.h"
#include "./classes/FileIO.h"
#include "./classes/Buffer.h"
#include "./classes/Silkscreen.h"

#include "./classes/Graph.h"
using namespace std;

int main(int argc, char **argv)
{
    double assemblygap = 0.0;
    double coppergap = 0.0;
    double silkscreenlen = 0.0;
    FileIO File;
    File.Read_File(argv[1]);
    assemblygap = File.File_to_Parameter();
    coppergap = File.File_to_Parameter();
    silkscreenlen = File.File_to_Parameter();
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