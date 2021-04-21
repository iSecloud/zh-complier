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

    //总定义
    void def(); 
    void deftail();
    Var* init();

    //变量定义
    Var *norvardef();
    void varlist();
    void vardef();

    //函数定义
    void fundef();
    void funtail();
    void funbody();
    void fungrogram();
    void localdef();
    

    bool match(int need); //判断是否匹配
    void move(); //得到token
 
    Lex &lexer; 
    Token *lookahead;

public:
    Parser(Lex &lexer);
    ~Parser();

    void Analysis();
};