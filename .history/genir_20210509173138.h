#pragma once
#include "common.h"
#include "symbol.h"

class GenIR //生成中间代码
{
    static int labelNum;

public:
    void genFunHead(Fun* fun); //产生函数rukou

};