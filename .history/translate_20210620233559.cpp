#include "translate.h"

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
        obj.objCode("mov", "ip", "sp");
        obj.objCode("stmfd", "sp!", "{fp, ip, lr, pc}");
    }
}

void Translate::transArm()
{
    for(auto i: intercode)
        translate(i);
}