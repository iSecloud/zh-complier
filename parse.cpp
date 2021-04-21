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

void Parser::recovery(bool real, Err errCode)
{
    Error::showError(errCode);
    //不可以与follow集匹配
    if(!real) move(); 
}