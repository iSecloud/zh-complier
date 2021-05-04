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
    s["aba"].push_back(3);
    for(auto i: s["aba"]) printf("%d ", i);
    return 0;
}
