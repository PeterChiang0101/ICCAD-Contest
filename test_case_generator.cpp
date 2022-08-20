#include <bits/stdc++.h>

using namespace std;

fstream open_file(int);
void write_file(fstream);

int main(){
    for(int i = 0; i < 360; i++){
        double x = 0;
        double y = 0;
        
        //fstream Output;
        //Output = open_file(i);
        //write_file(Output);
        
    }
}

fstream open_file(int index)
{
    fstream Output;
    string filename;
    filename = "testcase_" + to_string(index+1) + ".txt";
    Output.open(filename, ios::out);

    if (!Output)
    {
        cout << "Error: Cannot open file" << endl;
    }
}

void write_file(fstream Output)
{
    Output << "assemblygap,0.5" << endl
            << "coppergap,0.127" << endl
            << "silkscreenlen,0.2" << endl;
    Output << "assembly" << endl
            << "arc,4.5000,4.0000,4.0000,4.0000,4.2500,4.0000,CCW" << endl;
    double x = 0;
    double y = 0;
    
}