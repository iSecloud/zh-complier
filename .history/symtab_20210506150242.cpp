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
        varList.push_back(var->getName());
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
    //TODO 变量初始化 和代码生成有关
}

void SymTab::addStr(Var* &var)
{
    for(auto i: strTab)
        if(i.second->getStr() == var->getStr())
        {
            delete var;
            var = i.second;
            return;
        }
    strTab[var->getName()] = var;
}

Var* SymTab::getVar(string name)
{
    Var* ans = NULL;
    if(varTab.find(name) != varTab.end())
    {
        int maxlen = 0;
        int scopeLen = scopePath.size();
        for(auto i: *varTab[name])
        {
            if(i->getScope().size() > scopeLen) continue;
            int len = i->getScope().size();
            if(i->getScope()[len - 1] == scopePath[len - 1])
                if(maxlen < len) maxlen = len, ans = i; 
        }
    }
    if(!ans) Error::showError(VAR_DEF_NONE);
    return ans; 
}

void SymTab::decFun(Fun* fun)
{
    fun->setExtern(true);
    if(funTab.find(fun->getName()) == funTab.end())
    {
        funTab[fun->getName()] = fun;
        funList.push_back(fun->getName());
    }
    else
    {
        if(!funTab[fun->getName()]->matchFun(fun))
            Error::showError(FUN_DEC_ERR);
        delete fun; //删除以释放空间
    }
}

void SymTab::defFun(Fun* fun)
{
    if(fun->getExtern()) //extern声明的不能再定义
    {
        Error::showError(FUN_EXT_ERR);
        fun->setExtern(false);
    }
    if(funTab.find(fun->getName()) == funTab.end())
    {
        funTab[fun->getName()] = fun;
        funList.push_back(fun->getName());
    }
    else
    {
        Fun* f = funTab[fun->getName()];
        if(!f->getExtern()) //重定义
        {
            Error::showError(FUN_DEF_MUL);  
            //不允许重定义
        }
        else
        {
            if(!f->matchFun(fun))
                Error::showError(FUN_DEC_ERR);
            f->define(fun);
        }
        delete fun;
        fun = f;
    }
    curFun = fun;
    //TODO 产生函数入口
}

void SymTab::endDefFun()
{
    //TODO 产生函数出口
    curFun = NULL;
}

Fun* SymTab::getFun(string name, vector<Var* >&paraList)
{
    if(funTab.find(name) == funTab.end())
    {
        Error::showError(FUN_UN_DEC);
        return NULL;
    }
    Fun* fun = funTab[name];
    if(fun->getPara().size() != paraList.size())
    {
        Error::showError(FUN_CALL_ERR);
        return NULL;
    }
    for(int i = 0; i < paraList.size(); i ++)
        if(fun->getPara()[i]->getType() != paraList[i]->getType()) //TODO 用类型兼容来检查
        {
            Error::showError(FUN_CALL_ERR);
            return NULL;
        }
    return fun;
}

vector<int>& SymTab::getScope()
{
    return scopePath;
}