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

int calc(int a, int b)
{
    return a + b;
}

int main()
{
    int b = 2, a = 5;
    int c = calc(2, 3) + b;
    return 0;
}
