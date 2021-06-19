#include "obj.h"
#define emit(args...) arm_code.push_back(new Arm(args))

Arm::Arm(string op, string res, string arg1, string arg2, string addition)
{
    dead = false;
    this->op = op; 
    this->res = res;
    this->arg1 = arg1;
    this->arg2 = arg2;
    this->addition = addition;
}

Arm::~Arm() 
{}

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
    if(arg2 != "") code +=  ", " + arg2;
    if(addition != "") code += "; " + addition;
}

string Obj::immNum(int num, bool f)
{
    stringstream ss;
    string s = "";
    ss << num;
    if(f) s = "#" + ss.str();
    else s = ss.str();
    return s;
}

Obj::Obj()
{
    arm_code.clear();
}

Obj::~Obj()
{
    for(auto code: arm_code)
        delete code;
}

void Obj::objCode(string op)
{
    emit(op);
}

void Obj::objCode(string op, string res)
{
    emit(op, res);
}

void Obj::objCode(string op, string res, string arg1)
{
    emit(op, res, arg1);
}

void Obj::objCode(string op, string res, string arg1, string arg2)
{
    emit(op, res, arg1, arg2);
}

void Obj::ldrFake(string reg, string name)
{
    emit("ldr", reg, "=" + name);
}

void Obj::ldrImm(string reg, int num)
{
    if(ARM::validConstExpr(num))
        emit("mov", reg, immNum(num));
    else
        ldrFake(reg, immNum(num, false)); //用ldr伪指令进行加载
}

void Obj::ldrBase(string reg, string baseReg, int offset, bool isChar)
{
    string base = baseReg;
    if(ARM::validOffset(offset))
        base += ", " + immNum(offset);
    else
    {

    }
}

void Obj::ldrVar(string reg, Var* var)
{
    if(!var) return;
    if(var->isConst()) //常量加载
    {
        if(var->isBaseType()) //数字或者字符
            ldrImm(reg, var->getVal());
        else //字符串
            ldrFake(reg, var->getName());
    }
    else //变量加载
    {
        bool isChar = var->isChar();
        //TODO 寄存器优化

    }
}