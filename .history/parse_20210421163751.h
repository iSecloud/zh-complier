#pragma once
#include "common.h"
#include "lex.h"

class Parser
{
    Lex &lexer;
    
public:
    Parser(lex &lexer);
    ~Parser();

    void Analysis();
};