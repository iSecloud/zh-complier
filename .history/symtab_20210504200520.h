#pragma once
#include "common.h"
#include "symbol.h"
#include "error.h"
using namespace std;

class SymTab
{
    //记录添加顺讯
    vector<string> varList;
    vector<string> funList;

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
    void enter();
    void leave();
    
    //变量表和函数表管理
    void addVar(Var* var);
    void addStr(Var* &var);
    Var* getVar(string name);
    void decFun(Fun* fun);
    void defFun(Fun* fun);
    void endDefFun();
    Fun* getFun(string name, vector<Var* >&paraList);

    //外部接口
    vector<int> &getScope();
};