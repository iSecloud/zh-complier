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

    bool tyepCheck(Var* lval, Var* rval); //类型匹配

    void genFunHead(Fun* fun); //产生函数入口
    void genFunTail(Fun* fun); //产生函数出口
    void genFunReturn(Var* ret); //产生return语句

    Var* genPtr(Var* var); //指针运算*p
    Var* genAddress(Var* var); //取值运算&p
    Var* genVal(Var* var); //拷贝
    Var* genAssign(Var* lval, Var* rval);
};