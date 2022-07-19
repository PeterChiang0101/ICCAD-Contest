#include <iostream>

#include "scorer.h"

using namespace std;

int main()
{
     // Scorer s("./PublicCase/PublicCase_B.txt", "./PublicCase/PublicCase_B_Ans.txt");
     Scorer s("./TestingCase/test_C.txt", "./TestingCase/test_C_Ans.txt");
     cout << setprecision(4) << fixed << s.Total_score() << endl;
     return 0;
}