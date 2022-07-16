#include <iostream>

#include "scorer.h"

using namespace std;

int main()
{
     Scorer s("./TestingCase/test_C.txt", "./TestingCase/test_C_Ans.txt");
     cout << s.Total_score() << endl;
     return 0;
}