#include "symtab.h"
#include "symbol.h"
#include "token.h"
#include "error.h"
#include "genir.h"

Var* SymTab::voidVar = NULL;
Var* SymTab::oneVar = NULL;
Var* SymTab::fourVar = NULL;
Var* SymTab::zeroVar = NULL;

SymTab::SymTab()
{
    voidVar = new Var();
    Token* tk;
    tk = new Num(0); zeroVar = new Var(tk); //delete tk;
    tk = new Num(1); oneVar = new Var(tk); //delete tk;
    tk = new Num(4); fourVar = new Var(tk); //delete tk;

    addVar(voidVar);
    addVar(zeroVar);
    addVar(oneVar);
    addVar(fourVar);

    ir = NULL;
    curFun = NULL;
    scopeNum = 0;
    scopePath.push_back(0);
}

SymTab::~SymTab()
{}

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
    var->toStringVar();
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
    if(ir->genVarInit(var) && curFun) curFun->locate(var); //常量不需要栈帧偏移
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

void SymTab::addCode(Quaternion *code)
{
    if(!curFun) delete code; //无效指令
    else curFun->addCode(code);
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
    ir->genFunHead(curFun);
}

void SymTab::endDefFun()
{
    ir->genFunTail(curFun);
    curFun->printInterCode(); 输出中间代码
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
        if(!GenIR::typeCheck(fun->getPara()[i], paraList[i])) 
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

vector<string>& SymTab::getVarList()
{
    return varList;
}

vector<string>& SymTab::getFunList()
{
    return funList;
}

void SymTab::setIR(GenIR *ir)
{
    this->ir = ir;
}

Fun* SymTab::getCurFun()
{
    return curFun;
}
