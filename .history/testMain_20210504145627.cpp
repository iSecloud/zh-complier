#include <bits/stdc++.h>
#define debug system("pause")
using namespace std;

extern int calc(int a, int b);

class test
{
    int a, b;
public:
    test()
    {
        a = 2;
        b = 3;
    }
};

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

int calc(int a, int b, int c)
{
    return a + b + c;
}

int main()
{
    int c = 5;
    int* d = &c;
    int* &b = d;
    printf("%d %d %d", b, c, calc(*b, c));
    test t;
    printf("%d", t.a);
    return 0;
}
