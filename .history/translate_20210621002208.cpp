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
        obj.objCode("stmfd", "sp!", "{fp, ip, lr, pc}");
        //TODO 函数重定向
        obj.objCode("sub", "fp", "ip", "#4");
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
    else if(op == OP_ASSIGN)
    {
        Var* arg1 = code->getArg1();
        Var* arg2 = code->getArg2();
        obj.ldrVar("r9", arg2);
        obj.objCode("mov", "r8", "r9");
        obj.strVar("r8", "r9", arg1);
    }
}

void Translate::transArm()
{
    for(auto i: intercode)
        translate(i);
}