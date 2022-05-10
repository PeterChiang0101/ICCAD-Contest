#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring> // strtok
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

const vector<string> split(const string& str, const char& delimiter);

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
    vector<string> ret; 
    int tmp=0;

    string assembly_line;
    file >> assembly_line >> assembly_line; // first one is ignored
    
    
    

    while (assembly_line != "copper")
    {
        struct segment master;
        if (assembly_line[0] == 'l') // reading line
        {
            
            ret = split(assembly_line, ',');
            for (auto& s : ret) 
            {
                if(s=="line") 
                {
                    master.is_line=1;
                    tmp=0;
                }
                else
                {
                    tmp++;
                    switch(tmp)
                    {
                        case 1:
                            master.x1=stof(s);
                            break;
                        case 2:
                            master.y1=stof(s);
                            break;
                        case 3:
                            master.x2=stof(s);
                            break;
                        case 4:
                            master.y2=stof(s);
                            break;
                    }
                }
                
            }
            master.center_x=0;
            master.center_y=0;
            master.direction=0;
            assembly.push_back(master);
        }
        else // reading arc
        {
            ret = split(assembly_line, ',');
            for (auto& s : ret) 
            {
                if(s=="arc") 
                {
                    master.is_line=0;
                    tmp=0;
                }
                else
                {
                    tmp++;
                    switch(tmp)
                    {   
                        case 1:
                            master.x1=stof(s);
                            break;
                        case 2:
                            master.y1=stof(s);
                            break;
                        case 3:
                            master.x2=stof(s);
                            break;
                        case 4:
                            master.y2=stof(s);
                            break;
                        case 5:
                            master.center_x=stof(s);
                            break;
                        case 6:
                            master.center_y=stof(s);
                            break;
                        case 7:
                            if(s=="CW")
                                master.direction=0;
                            else
                                master.direction=1;
                            break;
                        
                    }
                } 
            }
            assembly.push_back(master);
        }
        file >> assembly_line;
    }

    /* for(int i=0;i<10;i++){
        cout<<assembly[i].is_line<<" "<<assembly[i].x1<<" "<<assembly[i].y1<<" "<<assembly[i].x2<<" "<<assembly[i].y2<<" "
        <<assembly[i].center_x<<" "<<assembly[i].center_y<<" "<<assembly[i].direction<<endl;
    }*/


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

const vector<string> split(const string& str, const char& delimiter) {
    vector<string> result;
    stringstream ss(str);
    string tok;

    while (getline(ss, tok, delimiter)) {
        result.push_back(tok);
    }
    return result;
}