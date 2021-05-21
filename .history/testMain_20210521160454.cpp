#include <bits/stdc++.h>
// #include "lex.h"
// #include "token.h"
// #include "common.h"
// #include "error.h"
// #include "parse.h"
// #include "symtab.h"
// #include "symbol.h"
#define debug system("pause")
using namespace std;

int main()
{
    // const char *fname = "test.txt";
    // Scanner sc(fname);
    // Error err(&sc);
    // Lex lexer(sc);
    // SymTab symtab;
    // Parser parser(lexer, symtab);
    // parser.Analysis();
    int a = 10;
    int* p = &a;
    int** q = &p;
    *q ++;
    printf("%d %d %d %d %d %d", q, *q, **q, p, *p, a);
    return 0;
}
