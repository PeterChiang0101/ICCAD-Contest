#include <iostream>

#include "scorer.h"

using namespace std;

int main()
{
     Scorer s("./TestingCase/test_B.txt", "./TestingCase/test_B_Ans.txt");
     cout << "first_score: " << s.first_quarter() << endl
          << endl;
     cout << "second_score: " << s.second_quarter() << endl
          << endl;
     cout << "third_score: " << s.third_quarter() << endl
          << endl;
     cout << "fourth_score: " << s.fourth_quarter() << endl;
     return 0;
}