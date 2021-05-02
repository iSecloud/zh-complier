#include <bits/stdc++.h>
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
    int b = 2, c = 3;
    switch(int a = b + c)
    int a = isInFollow(vector<int>{2, 4, 5, 7});
    printf("%d", a);
    return 0;
}
