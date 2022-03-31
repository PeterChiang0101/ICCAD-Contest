#include <fstream>
#include <iostream>

using namespace std;

int first_quarter()
{
    return 0;
}

int second_quarter()
{
    return 0;
}

int third_quarter()
{
    return 0;
}

int fourth_quarter()
{
    return 0;
}
int main()
{
    /* ----------Variable declaration---------- */
    int first_score, second_score, third_score, fourth_score, total_score;
    string input_file_path = "input.txt";   // the given IC path
    string output_file_path = "output.txt"; // silkscreen path

    ifstream input_file, output_file;
    /* ------End of Variable declaration------- */

    /* -------------Reading files-------------- */

    //!!!not finished!!!
    input_file.open(input_file_path, ios::in);
    output_file.open(output_file_path, ios::in);

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