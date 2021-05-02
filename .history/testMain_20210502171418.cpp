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
    int e = a > b > c > d;
    debug;
    printf("%d", e);
    return 0;
}
