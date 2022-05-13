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
    vector<vector<segment>> copper;
    vector<segment> copper_master;
    vector<string> ret;
    struct segment master;
    bool type; // 0 = assembly, 1 = copper
    int element = 0; // which element of a segment
    int num = 0; // which copper

    string line;
    getline(file, line); //stange string???????
    
    while (getline(file, line))
    {
        cout<<line<<endl;
        if(line=="assembly")
        {
            type = 0;
            continue;
        }
        else if(line=="copper")
        {
            type = 1;
            num ++;
            if(num!=1)
            {
                cout<<"size0="<<copper_master.size()<<endl;
                copper.push_back(copper_master);
                copper_master.clear();
            }
            continue;
        }
        else
        {
            ret = split(line, ',');
            for (auto& s : ret) 
            {
                if(s=="line") // reading line
                {
                    master.is_line=1;
                    master.center_x=0;
                    master.center_y=0;
                    master.direction=0;
                    element=0;
                }
                else if(s=="arc") // reading arc
                {
                    master.is_line=0;
                    element=0;
                }
                else
                {
                    element++;
                    switch(element)
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
            if(type==0)
            {
                assembly.push_back(master);
            }
            else
            {
                //cout<<"num="<<num<<endl;
                copper_master.push_back(master);    
            }
        }
        

    }
    copper.push_back(copper_master);
    copper_master.clear();


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