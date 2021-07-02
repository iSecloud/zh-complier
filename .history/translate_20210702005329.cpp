#include "translate.h"
#include "armplat.h"
#include "intercode.h"
#include "symbol.h"

Translate::Translate(vector<Quaternion*> &intercode1, Obj& obj1)
    :intercode(intercode1), obj(obj1)
{}

void Translate::translate(Quaternion* code)
{
    Operator op = code->getOp();
    if(op == OP_LABEL)
    {
        string label = code->getLabel();
        if(label != "") obj.label(label);
    }
    else if(op == OP_ENTRY)
    {
        Fun* fun = code->getFun();
        obj.comment("Enter the fun " + fun->getName());
        obj.objCode("mov", "ip", "sp");
        obj.objCode("stmfd", "sp!", "{fp, ip, lr, pc}"); //将这四个寄存器压栈
        //TODO 函数重定向 
        obj.objCode("sub", "fp", "ip", "#4"); //使fp指向当前函数栈帧的栈底
        obj.comment("Allocate the stack space");
        obj.allocStack(fun, "r8");//分配函数栈帧
        obj.comment("Load the args");
        obj.ldrArg(fun);
        obj.comment("Enter the fun block");
    }
    else if(op == OP_EXIT)
    {
        Fun* fun = code->getFun();
        obj.comment("Exit the fun " + fun->getName());
        //TODO 函数重定向
        obj.objCode("ldmea", "fp", "{fp, sp, pc}");
        //obj.objCode("bx", "lr") 跳转
    }
    else if(op == OP_DEC)
    {
        Var* var = code->getArg1();
        if(var->isInitVar()) obj.initVar("r8", "r9", var);
    }
    else if(op == OP_SET)
    {
        Var* arg1 = code->getArg1();
        Var* res = code->getResult();
        obj.ldrVar("r8", res); 
        obj.ldrVar("r9", arg1);
        obj.strBase("r8", "r9", "r9", 0, arg1->isChar());
    }
    else if(op == OP_JMP)
    {
        obj.objCode("b", code->getTarget()->getLabel());
    }
    else if(op == OP_JNE)
    {
        obj.ldrVar("r8", code->getArg1());
        obj.ldrVar("r9", code->getArg2());
        obj.cmp("r8", "r8", "r9", "ne", "eq");
        obj.objCode("cmp", "r8", "#0");
        obj.objCode("bne", code->getTarget()->getLabel());
    }
    else if(op == OP_JT || op == OP_JF)
    {
        obj.ldrVar("r8", code->getArg1());
        obj.objCode("cmp", "r8", "#0");
        if(op == OP_JT)
            obj.objCode("bne", code->getTarget()->getLabel());
        else 
            obj.objCode("beq", code->getTarget()->getLabel());
    }
    else if(op == OP_ARG)
    {
        obj.ldrVar("r8", code->getArg1());
        obj.objCode("stmfd", "sp!", "{r8}");
    }
    else if(op == OP_PROC || op == OP_CALL)
    {
        Fun* fun = code->getFun();
        obj.callFun(fun, "r9");
        if(op == OP_CALL) obj.strVar("r8", "r9", code->getResult()); //回存返回结果
    }
    else if(op == OP_RET || op == OP_RETX)
    {
        string point = code->getTarget()->getLabel();
        obj.objCode("b", point); //不用记录参数x 因为结果已经保存到寄存器了
    }
    else if(op == OP_READ || op == OP_WRITE)
    {
        //printf("I got read_write\n"); system("pause");
        obj.in_out("r8", "r9", "r10", code->getArg1(), op);
    }
    else 
    {
        //运算符
        Var* res = code->getResult();
        Var* arg1 = code->getArg1();
        Var* arg2 = code->getArg2();
        //先读取可能出现的变量    
        if(arg1 != NULL) obj.ldrVar("r8", arg1);
        if(arg2 != NULL) obj.ldrVar("r9", arg2);
        
        if(op == OP_ASSIGN); //空语句
        else if(op == OP_ADD) obj.objCode("add", "r8", "r8", "r9");
        else if(op == OP_SUB) obj.objCode("sub", "r8", "r8", "r9");
        else if(op == OP_MUL) obj.objCode("mul", "r8", "r8", "r9");
        else if(op == OP_DIV) obj.callLibFun("__divsi3", "r8", "r8", "r9");
        else if(op == OP_MOD) obj.callLibFun("__modsi3", "r8", "r8", "r9");
        else if(op == OP_NEG) obj.objCode("rsb", "r8", "r8", "#0");
        else if(op == OP_AND) obj.objCode("and", "r8", "r8", "r9");
        else if(op == OP_OR) obj.objCode("oor", "r8", "r8", "r9");
        else if(op == OP_XOR) obj.objCode("eor", "r8", "r8", "r9");
        else if(op == OP_INV) obj.objCode("mvn", "r8", "r8");
        else if(op == OP_AAND) obj.logicAnd("r8", "r8", "r9");
        else if(op == OP_OOR) obj.logicOr("r8", "r8", "r9");
        else if(op == OP_NOT) obj.logicNot("r8", "r8");
        else if(op == OP_GE) obj.cmp("r8", "r8", "r9", "ge", "lt");
        else if(op == OP_GT) obj.cmp("r8", "r8", "r9", "gt", "le");
        else if(op == OP_LE) obj.cmp("r8", "r8", "r9", "le", "gt");
        else if(op == OP_LT) obj.cmp("r8", "r8", "r9", "lt", "ge");
        else if(op == OP_EQU) obj.cmp("r8", "r8", "r9", "eq", "ne");
        else if(op == OP_NEQU) obj.cmp("r8", "r8", "r9", "ne", "eq");
        else if(op == OP_LEA) obj.ldrLeaVar("r8", arg1);
        else if(op == OP_GET) obj.ldrBase("r8", "r8", 0, res->isChar());
        //回存结果
        obj.strVar("r8", "r9", res);
    }

}

void Translate::transArm()
{
    for(auto i: intercode)
        translate(i);
}