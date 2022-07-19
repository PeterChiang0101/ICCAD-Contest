#include <iostream>

#include "scorer.h"

using namespace std;

int main()
{
     Scorer s("./PublicCase/PublicCase_C.txt", "./PublicCase/PublicCase_C_Ans.txt");
     cout << setprecision(4) << fixed << s.Total_score() << endl;
     return 0;
}