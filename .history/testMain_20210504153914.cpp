#include <bits/stdc++.h>
#define debug system("pause")
using namespace std;

unordered_map<string, vector<int>> s;

enum Tag
{
    XOXO, OO, OK, MUL
};

int fun(int* &p)
{
    int b = 10;
    p = &b;
}

int main()
{
    int a = 5;
    int *p = &a;
    printf("%d", *p); debug;
    fun(p);
    printf("%d", *p); debug;
    return 0;
}
