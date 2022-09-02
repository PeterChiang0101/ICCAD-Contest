#include <bits/stdc++.h>
#include "./classes/Parameter.h"
#include "./classes/FileIO.h"
#include "./classes/Buffer.h"
#include "./classes/Silkscreen.h"
#include "./classes/scorer.h"

#include "./classes/Graph.h"
using namespace std;

int main(int argc, char **argv)
{
    double assemblygap = 0.0;
    double coppergap = 0.0;
    double silkscreenlen = 0.0;

    double gap_mutiplier = 1.0001;
    bool coppergap_not_valid = true, assemblygap_not_valid = true;

    FileIO File;
    File.Read_File(argv[1]);
    assemblygap = File.File_to_Parameter();
    coppergap = File.File_to_Parameter();
    silkscreenlen = File.File_to_Parameter();
    Graph assembly = File.Read_Assembly();
    vector<Graph> copper = File.Read_Copper();
    const double assemblygap_original = assemblygap;
    const double coppergap_original = coppergap;

    // while not satisfied with the result, increase the mutiplier
    while (coppergap_not_valid || assemblygap_not_valid)
    {
        if (gap_mutiplier < 1.001)
        {
            gap_mutiplier += 0.0001;
        }
        else if (gap_mutiplier < 1.01)
        {
            gap_mutiplier += 0.0005;
        }
        else if (gap_mutiplier < 1.1)
        {
            gap_mutiplier += 0.005;
        }
        else // multiplier >= 1.1
        {
            gap_mutiplier += 0.05;
        }

        if (coppergap_not_valid)
        {
            coppergap = gap_mutiplier * coppergap_original;
        }

        if (assemblygap_not_valid)
        {
            assemblygap = gap_mutiplier * assemblygap_original;
        }

        Buffer buffer(assemblygap, coppergap);
        Graph assembly_buffer = buffer.Assembly_Buffer(assembly);
        vector<Graph> copper_buffer = buffer.Copper_Buffer(copper);

        Silkscreen silkscreen(silkscreenlen);
        vector<Graph> silkscreen_buffer = silkscreen.Silkscreen_Assembly(assembly, assembly_buffer, copper_buffer);

        File.Write_File(silkscreen_buffer, argv[2]);

        Scorer s(argv[1], argv[2]);
        cout << setprecision(4) << fixed << s.Total_score(0) << endl;
        coppergap_not_valid = s.coppergap_not_valid;
        assemblygap_not_valid = s.assemblygap_not_valid;
    }
    return 0;
}