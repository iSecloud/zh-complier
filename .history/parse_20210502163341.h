#pragma once
#include "common.h"
#include "lex.h"
#include "token.h"
#include "symbol.h"

class Parser
{

    //主程序
    void Program();
    void Segment();
    Tag Type();

    //总定义
    void Def(Tag t, bool ext); 
    void DefTail(Tag t, bool ext, string name, bool ptr);
    Var *Init(Tag t, bool ext, string name, bool ptr);

    //变量定义
    Var *NorVarDef(Tag t, bool ext, string name, bool ptr);
    void VarList(Tag t, bool ext);
    Var *VarDef(Tag t, bool ext);

    //函数定义
    void FunDef(); //其实不需要
    void FunTail(Fun* fun);
    void FunBody();
    void FunProgram();
    void LocalDef();
    void Para(vector<Var*> &paraList);
    Var *ParaDef(Tag t);
    Var *ParaTail(Tag t, string name);
    void ParaList(vector<Var*> &paraList);

    //语句定义
    void Statement();
    void WhileStat();
    void DoWhileStat();
    void ForStat();
    void ForInit();
    void IfStat();
    void ElseStat();
    void SwitchStat();
    void CaseStat();
    void SecloudStat();
    int RadNum();
    void ReadStat();
    void WriteStat(int num);

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
