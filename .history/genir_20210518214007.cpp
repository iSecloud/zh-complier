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

void GenIR::genFunHead(Fun *fun)
{
    fun->enterScope();  
    symtab.addCode(new Quaternion(OP_ENTRY, fun)); //进入函数
    fun->setReturnPoint(new Quaternion()); //设置返回点
}

void GenIR::genFunTail(Fun *fun)
{
    symtab.addCode(fun->getReturnPoint()); //得到返回点
    symtab.addCode(new Quaternion(OP_EXIT, fun)); //退出函数
    fun->leaveScope();
}

void GenIR::genFunReturn(Var *ret)
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