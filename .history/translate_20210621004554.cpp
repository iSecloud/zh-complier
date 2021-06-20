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
    else if(op == OP_SET)
    {
        Var* arg1 = code->getArg1();
        Var* res = code->getResult();
        obj.ldrVar("r8", res); 
        obj.ldrVar("r9", arg1);
        obj.strBase("r8", "r9", "r9", 0, arg1->getPointer()->isChar());
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
}

void Translate::transArm()
{
    for(auto i: intercode)
        translate(i);
}