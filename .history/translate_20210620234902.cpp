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
        
    }
}

void Translate::transArm()
{
    for(auto i: intercode)
        translate(i);
}