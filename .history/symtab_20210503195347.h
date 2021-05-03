#pragma once
#include "common.h"
#include "symbol.h"
using namespace std;

class SymTab
{
    unordered_map<string, vector<Var*>*> varTab; //变量表
    unordered_map<string, Var*> strTab; //特殊 字符串表
    unordered_map<string, Fun*> funTab; //函数表
};