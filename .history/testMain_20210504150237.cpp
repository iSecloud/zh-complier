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
    if(s.find("abababab") == s.end()) s["abababab"].push_back(4456);
    else printf("teahhhh");
    return 0;
}
