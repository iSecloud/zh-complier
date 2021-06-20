#include "translate.h"

Translate::Translate(vector<Quaternion*> &intercode1, Obj& obj1)
    :intercode(intercode1), obj(obj1)
{}

void Translate::translate(Quaternion* code)
{

}

void Translate::transArm()
{
    for(auto i: intercode)
        translate(i)
}