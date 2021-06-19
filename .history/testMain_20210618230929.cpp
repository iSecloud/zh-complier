#include <bits/stdc++.h>
// #include "lex.h"
// #include "token.h"
// #include "common.h"
// #include "error.h"
// #include "parse.h"
// #include "symtab.h"
// #include "symbol.h"
// #include "genir.h"
#define debug system("pause")
using namespace std;

int main()
{
    // int a = 5;
    // int &b = a;
    // printf("%d", b);
    // freopen("ans.txt", "w", stdout);
    // const char *fname = "test.txt";
    // Scanner sc(fname);
    // Error err(&sc);
    // Lex lexer(sc);
    // SymTab symtab;
    // GenIR ir(symtab);
    // Parser parser(lexer, symtab, ir);
    // parser.Analysis();
    int c = 11, a = 5, b = 10;
    int* p = &a;
    p = p + 2;
    printf("%d %d %d %d %d", p, *p, &c, &a, &b);
    return 0;
}
