#include <bits/stdc++.h>
#include "lex.h"
#include "token.h"
#include "common.h"
#include "error.h"
#include "parse.h"
#include "symtab.h"
#include "symbol.h"
using namespace std;

unordered_map<string, vector<int>> s;

enum Tag
{
    XOXO, OO, OK, MUL
};

int fun(vector<int> &a)
{
    vector<int> b{1, 2, 3, 4, 5, 6, 7};
    for(auto i: b) printf("%d ", i); debug;
    b = a;
    for(auto i: b) printf("%d ", i); debug;
    for(auto i: a) printf("%d ", i); debug;
}

int main()
{
    vector<int> a{1, 2, 3, 4};
    fun(a);
    return 0;
}