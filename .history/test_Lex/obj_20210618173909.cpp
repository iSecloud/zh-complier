#include "obj.h"

Arm::Arm(string op, string res, string arg1, string arg2, string addition)
{
    dead = false;
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

string Arm::getArmCode()
{
    string code = "";
    if(dead) return code;
    if(op == "") return code;
    code += op;
    if(res != "") code += ", " + res;
    if(arg1 != "") code += ", " + arg1;
    if(arg2 != "") code +=  
}