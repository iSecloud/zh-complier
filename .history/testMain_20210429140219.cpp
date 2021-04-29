#include <bits/stdc++.h>
#include "lex.h"
#include "error.h"
#include "token.h"
using namespace std;

enum Tag{
    ID, MUL, XXOO
};

int main()
{
    vector<int> a = vector<int>{ID, MUL, XXOO};
    for(auto i: a) printf("%d ", i);
    return 0;
}
