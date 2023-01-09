#include <bits/stdc++.h>

# define PI acos(-1)

using namespace std;

struct Point
{
    float x1;
    float y1;

    float x2;
    float y2;
};


void write_file(vector<Point>);

int main(){
    vector<Point> point;
    double radius = 0.6000;

    for(int i = 0; i < 90 - 1; i++){
        Point x_y_point;
        double x = 0;
        double y = 0;
        x = 4.0000 - radius*sin(i*PI/180);
        y = 4.0000 - radius*cos(i*PI/180);
        x_y_point.x1 = x;
        x_y_point.y1 = y;
        x_y_point.x2 = INFINITY;
        x_y_point.y2 = INFINITY;
        point.push_back(x_y_point);
    }

    //miss 90 degree

    for(int i = 91; i < 180 + 1; i++)
    {
        Point x_y_point;
        double x = 0;
        double y = 6.0000;
        x = 4.0000 - 2.0000*tan((i-91)*PI/180);
        x_y_point.x1 = x;
        x_y_point.y1 = y;
        x_y_point.x2 = 4.5000;
        x_y_point.y2 = 6.0000;
        point.push_back(x_y_point);
    }
    write_file(point);
}

/*fstream open_file(int index)
{
    fstream Output;
    
}*/

void write_file(vector<Point> point)
{
    for(size_t i = 0; i < point.size(); i++)
    {
        fstream Output;

        string filename;
        filename = "D:\\iccad_contest\\ICCAD-Contest\\test_case\\testcase_" + to_string(i+1) + ".txt";
        cout << filename << endl;
        Output.open(filename, ios::out);

        if (!Output)
        {
            cout << "Error: Cannot open file" << endl;
        }

        Output << "assemblygap,0.05" << endl
                << "coppergap,0.127" << endl
                << "silkscreenlen,0.2" << endl;
        Output << "assembly" << endl
                << "arc,4.5000,4.0000,4.0000,4.0000,4.2500,4.0000,CCW" << endl;
        Output << "line,4.0000,4.0000," << point.at(i).x1 << "," << point.at(i).y1 << endl;

        if(point.at(i).x2 == INFINITY)
        {
            Output << "line," << point.at(i).x1 << "," << point.at(i).y1 << ",4.5000,4.0000" << endl;
        }
        else
        {
            Output << "line," << point.at(i).x1 << "," << point.at(i).y1 << "," << point.at(i).x2 << "," << point.at(i).y2 << endl;
            Output << "line," << point.at(i).x2 << "," << point.at(i).y2 << ",4.5000,4.0000" << endl;  
        }
            
    }
};