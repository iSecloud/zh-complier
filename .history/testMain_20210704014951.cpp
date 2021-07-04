#include <bits/stdc++.h>
#include <stdio.h>
#include "lex.h"
#include "token.h"
#include "common.h"
#include "error.h"
#include "parse.h"
#include "symtab.h"
#include "symbol.h"
#include "genir.h"
#define debug system("pause")
using namespace std;

int main()
{
    //freopen("ans.txt", "w", stdout);
    const char *fname = "test.txt";
    const char *AsmName = "asm.s";
    int opt = 0;
    Scanner sc(fname);
    Error err(&sc);
    Lex lexer(sc);
    SymTab symtab;
    GenIR ir(symtab);
    Parser parser(lexer, symtab, ir);
    parser.Analysis();
    symtab.printInterInfo(0); //打印未优化代码
    printf("Is it optimized? 0 NO 1 YES\n");
    scanf("%d", &opt);
    if(opt) //执行优化
    {
        symtab.genOptmize();
        symtab.printInterInfo(1);
    }
    symtab.genAssemble(opt);
    return 0;
}
