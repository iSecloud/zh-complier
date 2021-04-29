#include "parse.h"
#include "lex.h"
#include "token.h"
#include "symbol.h"
#include "error.h"

Parser::Parser(Lex &lexer1): lexer(lexer1) 
{}

Parser::~Parser() 
{}

void Parser::move()
{
    lookahead = lexer.getToken();
}

bool Parser::match(int need)
{
    if(lookahead->tag == need)
    {
        move();
        return true;
    }
    return false;
}

#define FOLLOW vector<int>
#define TYPE_FIRST KW_INT, KW_CHAR, KW_VOID
#define STAT_FIRST KW_WHILE, KW_DO, KW_SWITCH, KW_CASE, KW_DEFAULT, KW_IF, KW_ELSE, KW_FOR, KW_SECLOUD, KW_READ, KW_WRITE

//是否在follow集里面
bool Parser::isInFollow(vector<int> a)
{
    if(find(a.begin(), a.end(), lookahead->tag) != a.end()) 
        return true;
    else 
        return false;
}

void Parser::recovery(bool real, Err errCode)
{
    Error::showError(errCode);
    //不可以与follow集匹配
    if(!real) move(); 
}

void Parser::program()
{
    segment();
    program();
}

void Parser::segment()
{
    if(match(KW_EXTERN))
    {
        ...
    }
}
