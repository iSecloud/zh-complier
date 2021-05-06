#include <bits/stdc++.h>
#include "lex.h"
#include "token.h"
#include "common.h"
#include "error.h"
#include "parse.h"
#include "symtab.h"
#include "symbol.h"
#define debug system("pause")
using namespace std;

int main()
{
    const char *fname = "test.txt";
    Scanner sc(fname);
    Error err(&sc);
    Lex lexer(sc);
    SymTab symtab;
    Parser parser(lexer, symtab);
    //parser.Analysis();
    return 0;
}
