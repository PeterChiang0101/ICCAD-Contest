#include <iostream>

#include "scorer.h"

using namespace std;

int main()
{
     // Scorer s("./PublicCase/PublicCase_C.txt", "./PublicCase/PublicCase_C_Ans.txt");
     Scorer s("./TestingCase/test_C.txt", "./TestingCase/test_C_Ans.txt");
     cout << setprecision(4) << fixed << s.Total_score(true) << endl;
     return 0;
}