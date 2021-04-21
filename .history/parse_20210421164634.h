#pragma once
#include "common.h"
#include "lex.h"
#include "token.h"
#include "symbol.h"

class Parser
{

    //主程序
    void program();
    void segment();
    Tag type();

    //变量定义
    void  
    bool match(int need); //判断是否匹配
    void move(); //得到token
 
    Lex &lexer; 
    Token *lookahead;

public:
    Parser(Lex &lexer);
    ~Parser();

    void Analysis();
};