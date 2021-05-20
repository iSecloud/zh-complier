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

bool GenIR::typeCheck(Var* lval, Var* rval)
{
    //TODO 需要判断lavl和rval是否为空吗？
    if(lval->isBaseType() && rval->isBaseType()) 
        return true;
    if(!lval->isBaseType() && !rval->isBaseType())
        return lval->getType() == rval->getType();
    return false;
}

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
    Var* tmp = new Var(symtab.getScope(), var);
    symtab.addVar(tmp);
    if(var->getPointer() != NULL) //是一个引用类型
    {
        symtab.addCode(new Quaternion(OP_GET, tmp, var->getPointer()));
        //将var指针指向的值取给tmp
    }
    else
        symtab.addCode(new Quaternion(OP_ASSIGN, tmp, var)); //普通赋值
}

Var* GenIR::genAssign(Var* lval, Var* rval)
{
    if(!lval->getLeft())
    {
        Error::showError(NOT_LVAL_ERR);
        return lval;
    }
    if(!typeCheck(lval, rval))
    {
        Error::showError(TYPE_MATCH_ERR);
        return lval;
    }
    if(rval->getPointer() != NULL) rval = genVal(rval);
    if(lval->getPointer() != NULL)
        symtab.addCode(new Quaternion(OP_SET, rval, lval->getPointer()));
    else 
        symtab.addCode(new Quaternion(OP_ASSIGN, lval, rval));
    return lval;
}

Var* GenIR::genAdd(Var* lval, Var* rval)
{
    Var* tmp = NULL;
    if(!lval->isBaseType()) swap(lval, rval);
    if(!lval->isBaseType())
    {
        Error::showError(CALC_VAL_ERR);
        return lval;
    }
    else if(!rval->isBaseType())
    {
        tmp = new Var(symtab.getScope(), rval);
        lval = genMul(lval, Var::getStep(rval));
    }
    else
        tmp = new Var(symtab.getScope(), KW_INT, false);
    symtab.addVar(tmp);
    symtab.addCode(new Quaternion(OP_ADD, tmp, lval, rval));
    return tmp;
}