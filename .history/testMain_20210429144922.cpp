#include <bits/stdc++.h>
using namespace std;

enum Tag{
    ID, MUL, XXOO
};

int main()
{
    vector<int> a{1, 2, 3, 4, 5};
    for(auto i: a) printf("%d ", i);
    if(find(a.begin(), a.end(), MUL) != a.end()) printf("1++++");
    return 0;
}
