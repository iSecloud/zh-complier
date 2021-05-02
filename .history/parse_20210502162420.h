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
    void def(Tag t, bool ext); 
    void deftail(Tag t, bool ext, string name, bool ptr);
    Var *init(Tag t, bool ext, string name, bool ptr);

    //变量定义
    Var *norvardef(Tag t, bool ext, string name, bool ptr);
    void varlist(Tag t, bool ext);
    Var *vardef(Tag t, bool ext);

    //函数定义
    void fundef(); //其实不需要
    void funtail(Fun* fun);
    void funbody();
    void funprogram();
    void localdef();
    void para(vector<Var*> &paraList);
    Var *paradef(Tag t);
    Var *paratail(Tag t, string name);
    void paralist(vector<Var*> &paraList);

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
    int RadNum();
    void readstat();
    void writestat(int num);

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
    Var *idexpr(string name);
    void realarg();
    void arglist();

    bool match(int need); //判断是否匹配
    void move(); //得到token
    void recovery(bool real, Err errCode); //错误恢复
    bool isInFollow(vector<int> a); 
    Lex &lexer; 
    Token *lookahead;

public:
    Parser(Lex &lexer);
    ~Parser();

    void Analysis();
};
