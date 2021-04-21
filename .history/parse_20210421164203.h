#pragma once
#include "common.h"
#include "lex.h"
#include "token.h"

class Parser
{
    bool match(int need); //判断是否匹配
    void move(); //得到token
 
    Lex &lexer; 
    Token *lookahead;

public:
    Parser(Lex &lexer);
    ~Parser();

    void Analysis();
};