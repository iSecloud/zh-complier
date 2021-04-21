#include "parse.h"
#include "lex.h"
#include "token.h"
#include "symbol.h"
#include "error.h"

Parser::Parser(Lex &lexer1): lexer(lexer1) 
{}

Parser::~Parser() 
{}


bool Parser::match(int need)
{
    if(lookahead->tag == need)
    {
        move();
        return true;
    }
    return false;
}