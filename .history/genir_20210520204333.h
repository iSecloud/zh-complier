#pragma once
#include "common.h"
#include "symbol.h"
#include "symtab.h"

class GenIR //生成中间代码
{
    SymTab &symtab;
    static int labelNum;

public:
    GenIR(SymTab &symtab1);
    ~GenIR();

    bool typeCheck(Var* lval, Var* rval); //类型匹配

    void genFunHead(Fun* fun); //产生函数入口
    void genFunTail(Fun* fun); //产生函数出口
    void genFunReturn(Var* ret); //产生return语句

    Var* genSigOp(Tag opt, Var* lval); //单目运算
    Var* genBinOp(Tag opt, Var* lval, Var* rval); //双目运算
    Var* genPtr(Var* var); //指针运算*p
    Var* genAddress(Var* var); //取值运算&p
    Var* genVal(Var* var); //拷贝
    Var* genAssign(Var* lval, Var* rval); //赋值运算
    Var* genAdd(Var* lval, Var* rval); // +
    Var* genSub(Var* lval, Var* rval); // -
    Var* genMul(Var* lval, Var* rval); // *
    Var* genAand(Var* lval, Var* rval); //逻辑&&
    Var* genOor(Var* lval, Var* rval); //逻辑||
    Var* genEqu(Var* lval, Var* rval); // ==
    Var* genNequ(Var* lval, Var* rval); // !=
    Var* genDiv(Var* lval, Var* rval); // /
    Var* genMod(Var* lval, Var* rval); // %
    Var* genAnd(Var* lval, Var* rval); // & 
    Var* genOr(Var* lval, Var* rval); // |
    Var* genXor(Var* lval, Var* rval); // ^
    Var* genGe(Var* lval, Var* rval); // >=
    Var* genGt(Var* lval, Var* rval); // > 
    Var* genLe(Var* lval, Var* rval); // <= 
    Var* genLt(Var* lval, Var* rval); // < 
};