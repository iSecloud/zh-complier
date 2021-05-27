#include "genir.h"
#include "common.h"
#include "symbol.h"
#include "symtab.h"
#include "error.h"
#include <sstream>


int GenIR::labelNum = 0;

string GenIR::genLb() //生成唯一的全局标签
{
    string lb = "@L";
    stringstream ss;
    labelNum ++;
    ss << labelNum;
    return lb + ss.str(); 
}

GenIR::GenIR(SymTab &symtab1): symtab(symtab1)
{
    labelNum = 0;
    symtab.setIR(this); //绑定当前的IR
    labelPush(NULL, NULL); //作用标签初始化
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
        if(ret->getPointer() != NULL) ret = genVal(ret);
        symtab.addCode(new Quaternion(OP_RETX, rePoint, ret));
    }
}

void GenIR::genPara(Var* arg)
{
    if(arg->getPointer() != NULL)
        arg = genVal(arg);
    symtab.addCode(new Quaternion(OP_ARG, arg));
}

Var* GenIR::genCall(Fun* fun, vector<Var*>& args)
{
    if(!fun) return NULL;
    for(int i = args.size() - 1; ~i; i --) //逆向传递参数
        genPara(args[i]);
    if(fun->getType() == KW_VOID)
    {
        symtab.addCode(new Quaternion(OP_PROC, fun));
        return SymTab::voidVar;
    }
    else
    {
        Var* tmp = new Var(symtab.getScope(), fun->getType(), false);
        symtab.addCode(new Quaternion(OP_CALL, fun, tmp));
        symtab.addVar(tmp);
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
    return tmp;
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
    return lval; //忽略其他其他运算符
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
    else if(!rval->isBaseType()) //处理p + 1这种情况, 此时p是指针
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
    if(var->isBaseType())
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

Var* GenIR::genSigOpLeft(Tag opt, Var* var)
{
    if(var == NULL) return NULL;
    if(var->getType() == KW_VOID) //void不能参加运算
    {
        Error::showError(CALC_VAL_ERR);
        return NULL;
    }
    if(opt == AND) return genAddress(var);
    if(opt == MUL) return genPtr(var);
    if(opt == INC) return genIncLeft(var);
    if(opt == DEC) return genDecLeft(var);
    if(var->getPointer() != NULL) var = genVal(var); 
    if(opt == NNOT) return genNot(var); //逻辑!
    if(!var->isBaseType())
    {
        Error::showError(CALC_VAL_ERR);
        return var;
    }
    if(opt == SUB) return genNeg(var); 
    if(opt == NOT) return genInv(var);
    return var; //未知运算符忽略
}

Var* GenIR::genIncLeft(Var* var)
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

Var* GenIR::genDecLeft(Var* var)
{
    if(!var->getLeft())
    {
        Error::showError(CALC_VAL_ERR);
        return var;
    }
    //考虑-- *p
    if(var->getPointer() != NULL)
    {
        Var* tmp1 = genVal(var); //取值*p
        Var* tmp2 = genSub(tmp1, Var::getStep(var)); //+1
        genAssign(var, tmp2); //存回来
    }
    else
    {
        symtab.addCode(new Quaternion(OP_SUB, var, var, Var::getStep(var)));
        // Var* tmp1 = genSub(var, Var::getStep(var));
        // genAssign(var, tmp1);
    }
    return var;
}

Var* GenIR::genNot(Var* var)
{
    Var* tmp = new Var(symtab.getScope(), KW_INT, false);
    symtab.addVar(tmp);
    symtab.addCode(new Quaternion(OP_NOT, tmp, var));
    return tmp;
}

Var* GenIR::genNeg(Var* var)
{
    Var* tmp = new Var(symtab.getScope(), KW_INT, false);
    symtab.addVar(tmp);
    symtab.addCode(new Quaternion(OP_NEG, tmp, var));
    return tmp;
}

Var* GenIR::genInv(Var* var)
{
    Var* tmp = new Var(symtab.getScope(), KW_INT, false);
    symtab.addVar(tmp);
    symtab.addCode(new Quaternion(OP_INV, tmp, var));
    return tmp;
}

Var* GenIR::genSigOpRight(Tag opt, Var* var)
{
    if(var == NULL) return NULL;
    if(var->getType() == KW_VOID) //void不能参加运算
    {
        Error::showError(CALC_VAL_ERR);
        return NULL;
    }
    if(opt == ADD) return genIncRight(var);
    if(opt == DEC) return genDecRight(var);
    return var; //未知运算符忽略
}

Var* GenIR::genIncRight(Var* var)
{
    if(!var->getLeft())
    {
        Error::showError(CALC_VAL_ERR);
        return var;
    }
    Var* tmp1 = genVal(var); //拷贝
    //考虑(*p) ++, 而不是*(p ++) 注意这里我们认为*的运算级更高.
    if(var->getPointer() != NULL)
    {
        Var* tmp2 = genAdd(tmp1, Var::getStep(var)); //+1
        genAssign(var, tmp2); //存回来
    }
    else
    {
        symtab.addCode(new Quaternion(OP_ADD, var, var, Var::getStep(var)));
        // Var* tmp1 = genAdd(var, Var::getStep(var));
        // genAssign(var, tmp1);
    }
    return tmp1;
}

Var* GenIR::genDecRight(Var* var)
{
    if(!var->getLeft())
    {
        Error::showError(CALC_VAL_ERR);
        return var;
    }
    Var* tmp1 = genVal(var);
    if(var->getPointer() != NULL)
    {
        Var* tmp2 = genSub(tmp1, Var::getStep(var));
        genAssign(var, tmp2);
    }
    else
    {
        symtab.addCode(new Quaternion(OP_SUB, var, var, Var::getStep(var)));
        // Var* tmp1 = genSub(var, Var::getStep(var));
        // genAssign(var, tmp1);  
    }
    return tmp1;
}

Var* GenIR::genArray(Var* arr, Var* index) //考虑*(arr + index)
{
    if(!arr || !index) return NULL; //报错？
    if(arr->getType() == KW_VOID || index->getType() == KW_VOID)
    {
        Error::showError(ARR_IDX_ERR);
        return NULL;
    }
    if(arr->isBaseType() || !index->isBaseType())
    {
        Error::showError(ARR_IDX_ERR);
        return NULL;
    }
    return genPtr(genAdd(arr, index)); //产生*(arr + index)的指针
}

void GenIR::genIfHead(Var* condition, Quaternion*& _else)
{
    _else = new Quaternion();
    if(condition != NULL)
    {
        if(condition->getPointer() != NULL)
        {
            condition = genVal(condition);
            symtab.addCode(new Quaternion(OP_JF, _else, condition));
        }
    }
}

void GenIR::genElseHead(Quaternion*& _else, Quaternion*& _exit)
{
    _exit = new Quaternion();
    symtab.addCode(new Quaternion(OP_JMP, _exit));
    symtab.addCode(_else);
}

void GenIR::genElseTail(Quaternion*& _exit)
{
    symtab.addCode(_exit);
}

void GenIR::genIfTail(Quaternion*& _else)
{
    symtab.addCode(_else);
}

void GenIR::genSwitchHead(Quaternion*& _exit)
{
    _exit = new Quaternion();
    labelPush(NULL, _exit);
}

void GenIR::genCaseHead(Var* condition, Var* lit, Quaternion*& _case)
{
    _case = new Quaternion();
    //TODO 需要处理lit和condition的引用吗？
    symtab.addCode(new Quaternion(OP_JNE, _case, condition, lit));
}

void GenIR::genCaseTail(Quaternion*& _case)
{
    symtab.addCode(_case);
}

void GenIR::genSwitchTail(Quaternion*& _exit)
{
    symtab.addCode(_exit);
    labelPop();
}

void GenIR::genWhileHead(Quaternion*& _while, Quaternion*& _exit)
{
    _while = new Quaternion();
    _exit = new Quaternion();
    symtab.addCode(_while);
    labelPush(_while, _exit);
}

void GenIR::genWhilecond(Var* condition, Quaternion*& _exit)
{
    if(condition != NULL)
    {
        if(condition->getType() == KW_VOID)
            condition = Var::getTrue(); //Symtab::oneVar
        else if(condition->getPointer() != NULL)
            condition = genVal(condition);
        symtab.addCode(new Quaternion(OP_JF, _exit, condition));
    }
}

void GenIR::genWhileTail(Quaternion*& _while, Quaternion*& _exit)
{
    symtab.addCode(new Quaternion(OP_JMP, _while));
    symtab.addCode(_exit);
    labelPop();
}

void GenIR::genDoWhileHead(Quaternion*& _do, Quaternion*& _exit)
{
    _do = new Quaternion();
    _exit = new Quaternion();
    symtab.addCode(_do);
    labelPush(_do, _exit);
}

void GenIR::genDoWhileTail(Quaternion*& _do, Quaternion*& _exit, Var* condition)
{
    if(condition != NULL)
    {
        if(condition->getType() == KW_VOID)
            condition = Var::getTrue();
        else if(condition->getPointer() != NULL)
            condition = genVal(condition);
        symtab.addCode(new Quaternion(OP_JT, _do, condition));
    }
    symtab.addCode(_exit);
    labelPop();
}

void GenIR::genForHead(Quaternion*& _for, Quaternion*& _exit, Quaternion*& _block)
{
    _for = new Quaternion();
    _exit = new Quaternion();
    _block = new Quaternion();
    symtab.addCode(_for);
}

void GenIR::genForStep(Quaternion*& _exit, Quaternion*& _block, Quaternion*& _step, Var* condition)
{
    _step = new Quaternion();
    if(condition != NULL)
    {
        if(condition->getType() == KW_VOID)
            condition = Var::getTrue();
        else if(condition->getPointer() != NULL)
            condition = genVal(condition);
        symtab.addCode(new Quaternion(OP_JF, _exit, condition));
        symtab.addCode(new Quaternion(OP_JMP, _block));
    }
    symtab.addCode(_step);
    labelPush(_step, _exit);  
}

void GenIR::genForBlockHead(Quaternion*& _for, Quaternion*& _block)
{
    symtab.addCode(new Quaternion(OP_JMP, _for));
    symtab.addCode(_block);
}

void GenIR::genForBlockTail(Quaternion*& _step, Quaternion*& _exit)
{
    symtab.addCode(new Quaternion(OP_JMP, _step));
    symtab.addCode(_exit);
    labelPop();
}

void GenIR::labelPush(Quaternion* inlb, Quaternion* outlb)
{
    InLabels.push_back(inlb);
    OutLabels.push_back(outlb);
}

void GenIR::labelPop()
{
    InLabels.pop_back();
    OutLabels.pop_back();
}

void GenIR::genBreak()
{
    Quaternion* outlb = OutLabels.back();
    if(outlb != NULL) symtab.addCode(new Quaternion(OP_JMP, outlb));
    else Error::showError(BREAK_IS_ERR);
}

void GenIR::genContinue()
{
    Quaternion* inlb = InLabels.back();
    if(inlb != NULL) symtab.addCode(new Quaternion(OP_JMP, inlb));
    else Error::showError(CONTINUE_IS_ERR);
}

void GenIR::genRead(Var* var)
{
    symtab.addCode(new Quaternion(OP_READ, var));
}

void GenIR::genWrite(Var* var)
{
    symtab.addCode(new Quaternion(OP_WRITE, var));
}

bool GenIR::genVarInit(Var* var)
{
    if(var->getName()[0] == '<') 
        return false;
    symtab.addVar(new Quaternion(OP_DEC, var)); //声明变量
    if(var.setInit())
        genBinOp(ASSIGN, var, var->initVal);
        
}