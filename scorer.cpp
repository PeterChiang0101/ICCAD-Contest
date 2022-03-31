#include <fstream>
#include <iostream>
#include <list>
#include <vector>

using namespace std;

struct boarder
{
    float x1{-100000};
    float x2{-100000};
    float y1{-100000};
    float y2{-100000};

    // below only for "arc", set all zero for "line"
    float center_x{-100000}, center_y{-100000};
    bool direction{0}; // 0 = ClockWise(CW), 1 = ConterClockwise(CCW)
};

vector<boarder> *extended_y() // to extend the assembly with length of "y"
{
}

int first_quarter() // Raymond
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
    int first_score, second_score, third_score, fourth_score, total_score;
    static string input_file_path = "input.txt";   // the given IC path
    static string output_file_path = "output.txt"; // silkscreen path
    float assemblygap, coppergap, silkscreenlen;   // read the input, given by input.txt()
    boarder temp_boarder;

    ifstream input_file, output_file;
    list<boarder> assembly;
    list<list<boarder>> copper;
    // vector<boarder> *copper = new vector<boarder>[5];     //  unused

    /* ------End of Variable declaration------- */

    /* -------------Reading files-------------- */

    //!!!not finished!!!
    input_file.open(input_file_path, ios::in);
    output_file.open(output_file_path, ios::in);
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
    first_score = first_quarter();
    second_score = second_quarter();
    third_score = third_quarter();
    fourth_score = fourth_quarter();
    /* --------End of Calling Functions-------- */

    total_score = first_score + second_score + third_score + fourth_score;

    /* --------------Score output-------------- */
    cout << "first_score: " << first_score << endl
         << "second_score: " << second_score << endl
         << "third_score: " << third_score << endl
         << "fourth_score: " << fourth_score << endl;
    cout << "The score of this silkscreen is: " << total_score << " / 100" << endl;
    /* -----------End of Score output---------- */
}