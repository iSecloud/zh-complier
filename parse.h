#pragma once
#include "common.h"
#include "lex.h"
#include "token.h"
#include "symbol.h"
#include "symtab.h"
#include "genir.h"
#include "error.h"

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
    void ElseStat(Quaternion *_else, Quaternion *_exit);
    void SwitchStat();
    void CaseStat();
    void SecloudStat();
    int RadNum();
    void ReadStat();
    void WriteStat(int num);

    //表达式定义 lval表示左值 rval表示右值
    //命名还是采用全小写吧...
    Var *expr();
    Var *altexpr();
    Var *literal();
    Var *assexpr();
    Var *asstail(Var *leftVal); 
    Var *oorexpr();
    Var *oortail(Var *leftVal);
    Var *aandexpr();
    Var *aandtail(Var *leftVal);
    Var *orexpr();
    Var *ortail(Var *leftVal);
    Var *xorexpr();
    Var *xortail(Var *leftVal);
    Var *andexpr();
    Var *andtail(Var *leftVal);
    Var *cmpexpr();
    Var *cmptail(Var *leftVal);
    Tag cmps();
    Var *aloexpr();
    Var *alotail(Var *leftVal);
    Tag alos();
    Var *item();
    Var *itemtail(Var *leftVal);
    Tag its();
    Var *factor();
    Tag lop();
    Tag rop();
    Var *val();
    Var *element();
    Var *idexpr(string name);
    void realarg(vector<Var*> &argList);
    void arglist(vector<Var*> &argList);

    bool match(int need); //判断是否匹配
    void move(); //得到token
    void recovery(bool real, Err errCode); //错误恢复
    bool isInFollow(vector<int> a); 
    Tag GetTag();
    Lex &lexer; 
    Token *lookahead;
    SymTab &symtab;
    GenIR &ir;

public:
    Parser(Lex &lexer1, SymTab &symtab1, GenIR &ir1);
    ~Parser();

    void Analysis();
};
