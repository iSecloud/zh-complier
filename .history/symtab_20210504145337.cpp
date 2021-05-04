#include "symtab.h"

void SymTab::enter()
{
    scopeNum ++;
    scopePath.push_back(scopeNum);
    if(curFun) curFun->enterScope(); //同时让函数创建一个新的作用域
}

void SymTab::leave()
{
    scopePath.pop_back();
    if(curFun) curFun->leaveScope(); //同理，让函数离开当前作用域
}

void SymTab::addVar(Var* v)
{
    if(varTab.find())
}