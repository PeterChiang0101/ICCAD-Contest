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
    int first_score, second_score, third_score, fourth_score, total_score;
    first_score = first_quarter();
    second_score = second_quarter();
    third_score = third_quarter();
    fourth_score = fourth_quarter();

    total_score = first_score + second_score + third_score + fourth_score;

    cout << "The score of this silkscreen is: " << total_score << " / 100" << endl;
}