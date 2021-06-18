#include "obj.h"

Arm::Arm(string op, string res, string arg1, string arg2, string addition)
{
    this->op = op; 
    this->res = res;
    this->arg1 = arg1;
    this->arg2 = arg2;
    this->addition = addition;
}

void Arm::setDead()
{
    dead = true;
} 