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
    Token *token = lexer.getToken();
    // printf("++++++++++++++++++++++++++"); system("pause");
    // if(token->tag == END) printf("++++++");
    // else printf("-------");
    while(token->tag != END)
    {
        string s = token->toString();
        cout << s << endl; //system("pause");
        //printf("+++++++"); system("pause");
        token = lexer.getToken();
    }
    SymTab symtab;
    Parser parser(lexer, symtab);
    parser.Analysis();
    printf("%d ", symtab.getVarList().size()); debug;
    for(auto i: symtab.getVarList()) cout << i << endl;
    return 0;
}
