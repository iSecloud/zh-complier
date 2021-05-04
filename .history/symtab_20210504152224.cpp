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

void SymTab::addVar(Var* var)
{
    if(varTab.find(var->getName()) == varTab.end())
    {
        //这里等价于创建一个新的key-val
        varTab[var->getName()] = new vector<Var* >;
        varTab[var->getName()]->push_back(var);
    }
    else
    {
        vector<Var*> &list = *varTab[var->getName()];
        int pos;
        for(pos = 0; pos < list.size(); pos ++)
            if(list[pos]->getScope().back() == var->getScope().back())
                break;
        if(pos == list.size() || var->getName()[0] == '<')
            varTab[var->getName()]->push_back(var);
        else
        {
            Error::showError(VAR_DEF_MUL);
            delete var; //释放空间
            return;
        }
    }
}