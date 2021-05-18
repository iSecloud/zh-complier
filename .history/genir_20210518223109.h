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

    void genFunHead(Fun *fun); //产生函数入口
    void genFunTail(Fun *fun); //产生函数出口
    void genFunReturn(Var *ret); //产生return语句
};