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
    int c = 5;
    int &b = 8c;
    return 0;
}
