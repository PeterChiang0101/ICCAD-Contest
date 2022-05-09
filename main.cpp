#include <fstream>
#include <iostream>
#include <string>
#include <vector>
// use boost library

using namespace std;

struct segment
{
    bool is_line; // 0 = arc, 1 = line
    float x1;
    float y1;
    float x2;
    float y2;
    // below is needed by arc, when deals with line set all to 0
    float center_x;
    float center_y;
    bool direction; // 0 = ClockWise(CW), 1 = ConterClockwise(CCW)
};

int main()
{
    fstream file;
    string assemblygap_str, coppergap_str, silkscreenlen_str;
    float assemblygap, coppergap, silkscreenlen;

    file.open("input.txt", ios::in);

    // the first three line of the file, defines parameters for silkscreen
    file >> assemblygap_str >> coppergap_str >> silkscreenlen_str;

    // assemblygap : the minimum distance between assembly and silkscreen
    assemblygap_str = assemblygap_str.substr(assemblygap_str.find(',') + 1);
    assemblygap = stof(assemblygap_str);

    // coppergap : the minimum distance between copper and silkscreen
    coppergap_str = coppergap_str.substr(coppergap_str.find(',') + 1);
    coppergap = stof(coppergap_str);

    // silkscreenlen : the minimum length of silkscreen
    silkscreenlen_str = silkscreenlen_str.substr(silkscreenlen_str.find(',') + 1);
    silkscreenlen = stof(silkscreenlen_str);

    vector<segment> assembly;

    string assembly_line;
    file >> assembly_line >> assembly_line; // first one is ignored

    while (assembly_line != "copper")
    {
        if (assembly_line[0] == 'l') // reading line
        {
        }
        else // reading arc
        {
        }
        file >> assembly_line;
    }
    // transform into data structure
    // the main IC uses polygon
    // arc uses linestring
    // pin uses polygon

    // calculate the silkscreen
    // ignore the arc first
    // maximize the usage of boost
    // buffer() , closest_point(), distance(), within()

    // arc needed to be treated manually

    // output
}