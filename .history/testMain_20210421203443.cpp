#include <bits/stdc++.h>
#include "lex.h"
#include "error.h"
using namespace std;

int main()
{
    freopen("ID_text.txt", "w", stdout);
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
    Error er;
    er.printErrorInfo(make_pair(5, 1), FILE_CLOSE_ERROR);
    return 0;
}
