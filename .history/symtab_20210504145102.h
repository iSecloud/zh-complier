#pragma once
#include "common.h"
#include "symbol.h"
using namespace std;

class SymTab
{
    unordered_map<string, vector<Var*>*> varTab; //变量表
    unordered_map<string, Var*> strTab; //特殊 字符串表
    unordered_map<string, Fun*> funTab; //函数表

    Fun* curFun; //当前函数
    int scopeNum; //全局路径编号
    vector<int> scopePath; //当前路径

public:

    //符号表初始化
    SymTab();
    ~SymTab();
    
    //作用域管理
    void addVar(Var* var);
    void enter();
    void leave();

};