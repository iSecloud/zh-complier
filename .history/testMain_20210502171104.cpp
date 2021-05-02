#include <bits/stdc++.h>
#define debug system("pause")
using namespace std;

enum Tag
{
    XOXO, OO, OK, MUL
};

bool isInFollow(vector<int> a)
{
    if(find(a.begin(), a.end(), MUL) != a.end()) 
        return true;
    else 
        return false;
}

int main()
{
    int b = 2, c = 3, a = 5, d = 7;
    a = b = c = d;
    debug;
    int a = isInFollow(vector<int>{2, 4, 5, 7});
    printf("%d", a);
    return 0;
}
