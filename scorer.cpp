//functions of scorer.h

#include <bits/stdc++.h>
#include "scorer.h"
#include "inputoutput.h"

using namespace std;

#define INPUT_PATH "./TestingCase/test_B.txt"
#define OUTPUT_PATH "./TestingCase/test_B_Ans.txt"




void Scorer::open_file()
{
    Input_Output A;

    Q_file.open(INPUT_PATH, ios::in);
    A_file.open(OUTPUT_PATH, ios::in);
    string assemblygap_str, coppergap_str, silkscreenlen_str;
    assemblygap = A.File_to_Parameter(assemblygap_str);
    coppergap = A.File_to_Parameter(coppergap_str);
    silkscreenlen = A.File_to_Parameter(silkscreenlen_str);
    assembly = A.Read_Assembly(Q_file);
    copper = A.Read_Copper(Q_file);
    silkscreen = Read_Silkscreen(A_file);
}

int Scorer::first_quarter(const vector<Segment> Assembly, const vector<Segment> silkscreen)
{
    Input_Output A;
    float Rectangular_area; // 絲印標示之座標極限值所構成之矩形面積
    float X_max, Y_max, X_min, Y_min; //絲印座標極限值

    float Y_area; // 零件外觀向外等比拓展Y之面積範圍
    vector<Segment> Assembly_push_out;
    
    float Answer_1;

    /* calculate Rectangular_area */
    X_max = silkscreen[0].x1; X_min = silkscreen[0].x2;
    Y_max = silkscreen[0].y1; Y_min = silkscreen[0].y2;

    for(int i = 0; i <= silkscreen.size(); i++)
    {
        if(silkscreen[i].x2 > X_max) X_max = silkscreen[i].x2;
        if(silkscreen[i].x1 > X_max) X_max = silkscreen[i].x1;
        if(silkscreen[i].x2 < X_min) X_min = silkscreen[i].x2;
        if(silkscreen[i].x1 < X_min) X_min = silkscreen[i].x1;

        if(silkscreen[i].y2 > Y_max) Y_max = silkscreen[i].y2;
        if(silkscreen[i].y1 > Y_max) Y_max = silkscreen[i].y1;
        if(silkscreen[i].y2 < Y_min) Y_min = silkscreen[i].y2;
        if(silkscreen[i].y1 < Y_min) Y_min = silkscreen[i].y1;

    }
    Rectangular_area = abs((X_max - X_min)*(Y_max - Y_min));
    
    /* calculate Y_area*/
    Assembly_push_out = A.Assembly_Buffer(Assembly, coppergap, assemblygap);

    float total_area = 0;
    int i;
    int j = i -1;
    for(i = 0; i <= Assembly_push_out.size(); i++)
    {
        if(i == 0) j = Assembly_push_out.size() - 1;
        else j = i - 1;
        total_area += (Assembly_push_out[j].x1 + Assembly_push_out[i].x1)*(Assembly_push_out[j].y1 - Assembly_push_out[i].y1);
    }
    /*
    for(i = 0; i <= Assembly_push_out.size(); i++)
    {
        if(!is_line)
        {

        }
        // undone //
    }
    */

    Answer_1 = (2-Rectangular_area/Y_area)*0.25;
    cout << "Answer_1 = " << Answer_1;
}

vector<Segment> Scorer::Read_Silkscreen(fstream &Input_File)
{
    Input_Output A;
    vector<Segment> Assembly;
    Segment part;
    vector<string> split_return;
    string line;
    getline(Input_File, line);

    while (getline(Input_File, line))
    {
        if (line == "assembly")
            continue;
        else
            part = A.String_to_Line(line);
        Assembly.push_back(part);
    }
    return Assembly;
}



/*struct boarder
{
    float x1{-100000};
    float x2{-100000};
    float y1{-100000};
    float y2{-100000};

    // below only for "arc", set all zero for "line"
    float center_x{-100000}, center_y{-100000};
    bool direction{0}; // 0 = ClockWise(CW), 1 = ConterClockwise(CCW)
};*/

/*vector<boarder> *extended_y() // to extend the assembly with length of "y"
{
}*/
/*int first_quarter() // Raymond
{
    return 0;
}

int second_quarter() // Willy Shius
{
    return 0;
}

int third_quarter() // peter // find nearest silkscreen for each copper and take average value
{
    return 0;
}

int fourth_quarter() // macoto //絲印標示與零件外觀之平均距離評分
{
    return 0;
}

int main()
{
    /* ----------Variable declaration---------- */
    /*int first_score, second_score, third_score, fourth_score, total_score;
    static string input_file_path = "input.txt";   // the given IC path
    static string output_file_path = "output.txt"; // silkscreen path
    float assemblygap, coppergap, silkscreenlen;   // read the input, given by input.txt()
    boarder temp_boarder;

    ifstream input_file, output_file;
    list<boarder> assembly;
    list<list<boarder>> copper;*/
    
    // vector<boarder> *copper = new vector<boarder>[5];     //  unused

    /* ------End of Variable declaration------- */

    /* -------------Reading files-------------- */

    //!!!not finished!!!
    //input_file.open(input_file_path, ios::in);
    //output_file.open(output_file_path, ios::in);
    /*@TODO
        !!function needed!! *split the input string*


        string temp;
        input_file >> temp
        if(temp == "assemblygap")
        {
            define assemblygap
        }else if(temp == "coppergap"){
            define coppergap
        }else if(temp == "silkscreenlen"){
            define silkscreenlen
        }else if(temp == "assembly"){
            switch current input object to assembly
        }else if (temp == "copper"){
            switch input object to copper / to next copper

        }else if (temp == "line"){
            file >> temp_boarder;
            list current.push_back(temp_boarder);
        }else if (temp == "arc"){
            file >> temp_boarder;
            list current.push_back(temp_boarder);

        }else{
            cerr << "Error!";
        }

    */

    /* ----------End of Reading Files---------- */

    /* -----------Calling Functions------------ */
    /*first_score = first_quarter();
    second_score = second_quarter();
    third_score = third_quarter();
    fourth_score = fourth_quarter();
    /* --------End of Calling Functions-------- */

    //total_score = first_score + second_score + third_score + fourth_score;

    /* --------------Score output-------------- */
    /*cout << "first_score: " << first_score << endl
         << "second_score: " << second_score << endl
         << "third_score: " << third_score << endl
         << "fourth_score: " << fourth_score << endl;
    cout << "The score of this silkscreen is: " << total_score << " / 100" << endl;*/
    /* -----------End of Score output---------- */
//}