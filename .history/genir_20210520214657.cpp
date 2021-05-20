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

Var* GenIR::genBinOp(Tag opt, Var* lval, Var* rval)
{
    if(!lval || !rval) return NULL; //为空则不进行运算,或者报错？
    if(lval->getType() == KW_VOID || rval->getType() == KW_VOID) //void函数不能参加运算
    {
        Error::showError(CALC_VAL_ERR);
        return NULL;
    }
    if(opt == ASSIGN) return genAssign(lval, rval); //赋值
    if(lval->getPointer() != NULL) lval = genVal(lval);
    if(rval->getPointer() != NULL) rval = genVal(rval);
    //&& || == != + -
    if(opt == AAND) return genAand(lval, rval);
    if(opt == OOR) return genOor(lval, rval);
    if(opt == ADD) return genAdd(lval, rval);
    if(opt == SUB) return genSub(lval, rval);
    if(opt == EQU) return genEqu(lval, rval);
    if(opt == NEQU) return genNequ(lval, rval);
    //以下运算只能是基本类型进行运算
    // * / % & | ^ > >= < <=
    if(!lval->isBaseType() || !rval->isBaseType()) 
    {
        Error::showError(CALC_VAL_ERR);
        return lval;
    }
    if(opt == MUL) return genMul(lval, rval);
    if(opt == DIV) return genDiv(lval, rval);
    if(opt == MOD) return genMod(lval, rval);
    if(opt == AND) return genAnd(lval, rval);
    if(opt == OR) return genOr(lval, rval);
    if(opt == XOR) return genXor(lval, rval);
    if(opt == GT) return genGt(lval, rval);
    if(opt == GE) return genGe(lval, rval);
    if(opt == LT) return genLt(lval, rval);
    if(opt == LE) return genLe(lval, rval);
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
    else if(!rval->isBaseType()) //处理*(p + 1)这种情况
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

Var* GenIR::genSub(Var* lval, Var* rval)
{
    Var* tmp = NULL;
    if(!rval->isBaseType()) //没有*(1 - p)这种操作
    {
        Error::showError(CALC_VAL_ERR);
        return lval;
    }
    else if(!lval->isBaseType())
    {
        tmp = new Var(symtab.getScope(), lval);
        rval = genMul(rval, Var::getStep(lval));
    }
    else
        tmp = new Var(symtab.getScope(), KW_INT, false); 
    symtab.addVar(tmp);
    symtab.addCode(new Quaternion(OP_SUB, tmp, lval, rval));
    return tmp;
}

Var* GenIR::genMul(Var* lval, Var* rval)
{
    //在前面已经判断了lval与rval的类型，这里便不在判断 剩下的同理
    Var* tmp = new Var(symtab.getScope(), KW_INT, false);
    symtab.addVar(tmp);
    symtab.addCode(new Quaternion(OP_MUL, tmp, lval, rval));
    return tmp;
}

Var* GenIR::genAand(Var* lval, Var* rval)
{
    Var* tmp = new Var(symtab.getScope(), KW_INT, false);
    symtab.addVar(tmp);
    symtab.addCode(new Quaternion(OP_AAND, tmp, lval, rval));
    return tmp;
}

Var* GenIR::genOor(Var* lval, Var* rval)
{
    Var* tmp = new Var(symtab.getScope(), KW_INT, false);
    symtab.addVar(tmp);
    symtab.addCode(new Quaternion(OP_OOR, tmp, lval, rval));
    return tmp;
}

Var* GenIR::genEqu(Var* lval, Var* rval)
{
    Var* tmp = new Var(symtab.getScope(), KW_INT, false);
    symtab.addVar(tmp);
    symtab.addCode(new Quaternion(OP_EQU, tmp, lval, rval));
    return tmp;
}

Var* GenIR::genNequ(Var* lval, Var* rval)
{
    Var* tmp = new Var(symtab.getScope(), KW_INT, false);
    symtab.addVar(tmp);
    symtab.addCode(new Quaternion(OP_NEQU, tmp, lval, rval));
    return tmp;
}

Var* GenIR::genDiv(Var* lval, Var* rval)
{
    //在前面已经判断了lval与rval的类型，这里便不在判断
    Var* tmp = new Var(symtab.getScope(), KW_INT, false);
    symtab.addVar(tmp);
    symtab.addCode(new Quaternion(OP_DIV, tmp, lval, rval));
    return tmp;
}

Var* GenIR::genMod(Var* lval, Var* rval)
{
    //在前面已经判断了lval与rval的类型，这里便不在判断
    Var* tmp = new Var(symtab.getScope(), KW_INT, false);
    symtab.addVar(tmp);
    symtab.addCode(new Quaternion(OP_MOD, tmp, lval, rval));
    return tmp;
}

Var* GenIR::genAnd(Var* lval, Var* rval)
{
    //在前面已经判断了lval与rval的类型，这里便不在判断
    Var* tmp = new Var(symtab.getScope(), KW_INT, false);
    symtab.addVar(tmp);
    symtab.addCode(new Quaternion(OP_AND, tmp, lval, rval));
    return tmp;
}

Var* GenIR::genOr(Var* lval, Var* rval)
{
    //在前面已经判断了lval与rval的类型，这里便不在判断
    Var* tmp = new Var(symtab.getScope(), KW_INT, false);
    symtab.addVar(tmp);
    symtab.addCode(new Quaternion(OP_OR, tmp, lval, rval));
    return tmp;
}

Var* GenIR::genXor(Var* lval, Var* rval)
{
    //在前面已经判断了lval与rval的类型，这里便不在判断
    Var* tmp = new Var(symtab.getScope(), KW_INT, false);
    symtab.addVar(tmp);
    symtab.addCode(new Quaternion(OP_XOR, tmp, lval, rval));
    return tmp;
}

Var* GenIR::genGe(Var* lval, Var* rval)
{
    //在前面已经判断了lval与rval的类型，这里便不在判断
    Var* tmp = new Var(symtab.getScope(), KW_INT, false);
    symtab.addVar(tmp);
    symtab.addCode(new Quaternion(OP_GE, tmp, lval, rval));
    return tmp;
}

Var* GenIR::genGt(Var* lval, Var* rval)
{
    //在前面已经判断了lval与rval的类型，这里便不在判断
    Var* tmp = new Var(symtab.getScope(), KW_INT, false);
    symtab.addVar(tmp);
    symtab.addCode(new Quaternion(OP_GT, tmp, lval, rval));
    return tmp;
}

Var* GenIR::genLe(Var* lval, Var* rval)
{
    //在前面已经判断了lval与rval的类型，这里便不在判断
    Var* tmp = new Var(symtab.getScope(), KW_INT, false);
    symtab.addVar(tmp);
    symtab.addCode(new Quaternion(OP_LE, tmp, lval, rval));
    return tmp;
}

Var* GenIR::genLt(Var* lval, Var* rval)
{
    //在前面已经判断了lval与rval的类型，这里便不在判断
    Var* tmp = new Var(symtab.getScope(), KW_INT, false);
    symtab.addVar(tmp);
    symtab.addCode(new Quaternion(OP_LT, tmp, lval, rval));
    return tmp;
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

Var* GenIR::genSigOp(Tag opt, Var* var)
{
    if(var == NULL) return NULL;
    if(var->getType() == KW_VOID) //void不能参加运算
    {
        Error::showError(CALC_VAL_ERR);
        return NULL;
    }
    if(opt == AND) return genAddress(var);
    if(opt == MUL) return genPtr(var);
    if(opt == INC) return genIncv(var);
    if(opt == DEC) return genDecv(var);
    if(var->getPointer() != NULL) var = genVal(var); //引用类型不能参加以下运算
    if(opt == SUB) return genNeg(var);
    if(opt == NNOT) return genNot(var);
    if(opt == NOT) return genInv(var);
}

Var* GenIR::genIncv(Var* var)
{
    if(!var->getLeft())
    {
        Error::showError(CALC_VAL_ERR);
        return var;
    }
    //考虑++ *p
    if(var->getPointer() != NULL)
    {
        Var* tmp1 = genVal(var); //取值*p
        Var* tmp2 = genAdd(tmp1, Var::getStep(var)); //+1
        genAssign(var, tmp2); //存回来
    }
    else
    {
        symtab.addCode(new Quaternion(OP_ADD, var, var, Var::getStep(var)));
        // Var* tmp1 = genAdd(var, Var::getStep(var));
        // genAssign(var, tmp1);
    }
    return var;
}

Var* GenIR::genDecv(Var* var)
{
    if(!var->getLeft())
    {
        Error::showError(CALC_VAL_ERR);
        return var;
    }
    //考虑++ *p
    if(var->getPointer() != NULL)
    {
        Var* tmp1 = genVal(var); //取值*p
        Var* tmp2 = genAdd(tmp1, Var::getStep(var)); //+1
        genAssign(var, tmp2); //存回来
    }
    else
    {
        symtab.addCode(new Quaternion(OP_ADD, var, var, Var::getStep(var)));
        // Var* tmp1 = genAdd(var, Var::getStep(var));
        // genAssign(var, tmp1);
    }
    return var;
}