#include <bits/stdc++.h>
#define debug system("pause")
using namespace std;

unordered_map<string, vector<int>> s;

enum Tag
{
    XOXO, OO, OK, MUL
};


int main()
{
    int a = 5;
    int &b = a;
    b = 10;
    printf("%d", a);
    return 0;
}
