#include <iostream>

#include "scorer.h"

using namespace std;

int main()
{
     // Scorer s("./PublicCase/PublicCase_A.txt", "./PublicCase/PublicCase_A_Ans.txt");
     Scorer s("./TestingCase/test_A.txt", "./TestingCase/test_A_Ans.txt");
     cout << setprecision(4) << fixed << s.Total_score(1) << endl;
     return 0;
}