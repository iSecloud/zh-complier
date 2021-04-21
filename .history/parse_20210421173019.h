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
    Var *init();

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
    void para();
    Var *paradef();
    Var *paratail();
    void paralist();

    //语句定义
    void statement();
    void whilestat();
    void dowhilestat();
    void forstat();
    void forinit();
    void ifstat();
    void elsestat();
    void switchstat();
    void casestat();
    void secloudstat();

    //表达式定义 lval表示左值 rval表示右值
    Var *expr();
    Var *altexpr();
    Var *literal();
    Var *assexpr();
    Var *asstail(Var *lval); 
    Var *oorexpr();
    Var *oortail(Var *lval);
    Var *aandexpr();
    Var *aandtail(Var *lval);
    Var *orexpr();
    Var *ortail(Var *lval);
    Var *xorexpr();
    Var *xortail(Var *lval);
    Var *andexpr();
    Var *andtail(Var *lval);
    Var *cmpexpr();
    Var *cmptail(Var *lval);
    Tag cmps();
    Var *aloexpr();
    Var *alotail(Var *lval);
    Tag alos();
    Var *item();
    Var *itemtail(Var *lval);
    Tag its();
    Var *factor();
    Tag lop();
    Tag rop();
    Var *val();
    Var *element();
    Var *literal();
    Var *idexpr();
    void realarg();
    void arglist();
    


    bool match(int need); //判断是否匹配
    void move(); //得到token
 
    Lex &lexer; 
    Token *lookahead;

public:
    Parser(Lex &lexer);
    ~Parser();

    void Analysis();
};