#include "genir.h"
#include "common.h"

int GenIR::labelNum = 0;

GenIR::GenIR(SymTab &symtab1): symtab(symtab1)
{
    labelNum = 0;
    symtab.setIR(this); //绑定当前的IR
}

GenIR::~GenIR()
{}

void GenIR::genFunHead(Fun* fun)
{
    fun->enterScope();  
    symtab.addCode(new Quaternion(OP_ENTRY, fun)); //进入函数
    fun->setReturnPoint(new Quaternion()); //设置返回点
}

void GenIR::genFunTail(Fun* fun)
{
    symtab.addCode(fun->getReturnPoint()); //得到返回点
    symtab.addCode(new Quaternion(OP_EXIT, fun)); //退出函数
    fun->leaveScope();
}

void GenIR::genFunReturn(Var* ret)
{
    if(!ret) return; //啥都没有就直接return ?
    Fun *curfun = symtab.getCurFun();
    if(ret->getType() == KW_VOID && curfun->getType() != KW_VOID)
    {
        Error::showError(FUN_RET_ERR);
        return;
    }
    if(ret->isBaseType() && curfun->getType() == KW_VOID)
    {
        Error::showError(FUN_RET_ERR);
        return;
    }
    Quaternion *rePoint = curfun->getReturnPoint();
    if(ret->getType() == KW_VOID)
        symtab.addCode(new Quaternion(OP_RET, rePoint));
    else
    {
        //TODO 如果ret是一个指针类型，则需要取值
        symtab.addCode(new Quaternion(OP_RETX, rePoint, ret));
    }
}

Var* GenIR::genPtr(Var* var)
{
    if(!var->isBaseType())
    {
        Error::showError(PTR_IS_ERR);
        return var;
    }
    Var* tmp = new Var(symtab.getScope(), var->getType(), false); //注意此时是*p, 所以(*p)是内容 p才是指针
    tmp->setLeft(true); //*p是可以作为左值运算的
    tmp->setPointer(var);
    symtab.addVar(tmp);
    return tmp;
}

Var* GenIR::genAddress(Var* var)
{
    if(!var->getLeft())
    {
        Error::showError(GET_ADDRESS_ERR);
        return var;
    }
    if(var->getPointer() != NULL) //这只处理&*p这种类型的运算
        return var->getPointer();
    else
    {
        Var* tmp = new Var(symtab.getScope(), var->getType(), true); //注意这里是指针,指向了地址
        symtab.addVar(tmp);
        symtab.addCode(new Quaternion(OP_LEA, tmp, var));
        return tmp;
    }
}

Var* GenIR::genVal(Var* var)
{
    Var* tmp = Var(symtab.getScope(), var);
    symtab.addVar(tmp);
    
}