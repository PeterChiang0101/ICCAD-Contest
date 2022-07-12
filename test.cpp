#include <iostream>

#include "scorer.h"

using namespace std;

int main()
{
    Scorer s;
    cout << s.first_quarter() << endl
         << endl;
    cout << s.second_quarter() << endl
         << endl;
    cout << s.third_quarter() << endl
         << endl;
    cout << s.fourth_quarter() << endl;
    return 0;
}