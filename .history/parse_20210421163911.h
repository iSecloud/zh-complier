#pragma once
#include "common.h"
#include "lex.h"
#include "token.h"

class Parser
{
    void move();
    Lex &lexer;
    Token *lookahead;

public:
    Parser(Lex &lexer);
    ~Parser();

    void Analysis();
};