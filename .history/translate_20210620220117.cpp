#include "translate.h"

Translate::Translate(vector<Quaternion*> &intercode1, Obj& obj1)
    :intercode(intercode1), obj(obj1)
{}

void Translate::translate(Quaternion* code)
{
    Operator op = code->getOp();
    if(op == "OP_LABEL")
    {
        string label = code->getLabel();
        
    }
}

void Translate::transArm()
{
    for(auto i: intercode)
        translate(i);
}