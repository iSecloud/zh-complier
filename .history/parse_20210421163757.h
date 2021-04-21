#pragma once
#include "common.h"
#include "lex.h"

class Parser
{
    Lex &lexer;
    
public:
    Parser(Lex &lexer);
    ~Parser();

    void Analysis();
};