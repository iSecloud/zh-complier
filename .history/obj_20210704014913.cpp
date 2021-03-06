#include "obj.h"
#include "symbol.h"
#include "symtab.h"
#include "armplat.h"
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
    if(res != "") code += " " + res;
    if(arg1 != "") code += ", " + arg1;
    if(arg2 != "") code +=  ", " + arg2;
    if(addition != "") code += "; " + addition;
    return code;
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
        ldrImm(reg, offset);
        base += ", " + reg;
    }
    base = "[" + base + "]";
    if(isChar) emit("ldrb", reg, base);
    else emit("ldr", reg, base);
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
        //TODO 寄存器优化
        bool isGlb = !(var->getOffset());
        bool isVar = !var->isArr();
        if(isGlb) //全局变量通过数据段查找
        {
            ldrFake(reg, var->getName());
            if(isVar) ldrBase(reg, reg, 0, var->isChar());
        }
        else //局部变量通过fp查找
        {
            if(isVar) 
                ldrBase(reg, "fp", var->getOffset(), var->isChar());
            else 
                leaStack(reg, var->getOffset());
        }
    }
}

void Obj::ldrLeaVar(string reg, Var* var)
{
    int offset = var->getOffset();
    if(!offset) //全局变量
        ldrFake(reg, var->getName());
    else
        leaStack(reg, var->getOffset());
}

void Obj::ldrArg(Fun* fun)
{
    vector<Var* > args = fun->getPara();
    //TODO 寄存器优化涉及
}

void Obj::strBase(string reg, string baseReg, string tmpReg, int offset, bool isChar)
{
    //printf("I got aabab!"); system("pause");
    string base = baseReg;
    if(ARM::validOffset(offset))
        base += ", " + immNum(offset);
    else
    {
        ldrImm(tmpReg, offset);
        base += ", " + reg;
    }
    //printf("I got hereeeeeeee!"); system("pause");
    base = "[" + base + "]";
    if(isChar) emit("strb", reg, base);
    else emit("str", reg, base);
}

void Obj::strVar(string reg, string tmpReg, Var* var)
{
    //保存寄存器值到变量
    if(!var) return;
    //TODO 寄存器优化
    bool isGlb = !(var->getOffset());
    if(isGlb) //全局变量通过数据段查找
    {
        ldrFake(tmpReg, var->getName());
        strBase(reg, tmpReg, tmpReg, 0, var->isChar());
    }
    else //局部变量通过fp查找
        strBase(reg, "fp", tmpReg, var->getOffset(), var->isChar());
    //TODO 分割优化
}

void Obj::leaStack(string reg, int offset)
{
    if(!ARM::validConstExpr(offset))
    {
        ldrImm(reg, offset);
        emit("add", reg, "fp", reg);
    }
    else
        emit("add", reg, "fp", immNum(offset));
}

void Obj::allocStack(Fun* fun, string tmpReg)
{
    //TODO 考虑重定向
    int base = ARM::stackBase;
    int offset = fun->getMaxDep() - base;
    //printf("++++++++++max %d+++++++++", fun->getMaxDep());
    // emit("stmfd", "sp!", "{fp,ip,lr,pc}");
    // emit("sub", "fp", "ip", "#4");
    if(!ARM::validConstExpr(offset))
    {
        ldrImm(tmpReg, offset);
        emit("sub", "sp", "sp", tmpReg);
    }
    else
        emit("sub", "sp", "sp", immNum(offset));
}

void Obj::callFun(Fun* fun, string tmpReg)
{
    string name = fun->getName();
    emit("bl", name);
    int offset = fun->getPara().size() * 4;
    if(!ARM::validConstExpr(offset))
    {
        ldrImm(tmpReg, offset);
        emit("add", "sp", "sp", tmpReg);
    }
    else
        emit("add", "sp", "sp", immNum(offset));
}

void Obj::callLibFun(string fun, string res2, string reg0, string reg1)
{
    emit("stmfd", "sp!", "{r0-r7}");
    emit("mov", "r0", reg0);
    emit("mov", "r1", reg1);
    emit("bl", fun);
    emit("mov", res2, "r0");
    emit("ldmfd", "sp!", "{r0-r7}");
}

void Obj::logicAnd(string resReg, string reg1, string reg2)
{
    emit("cmp", reg1, "#0");
    emit("moveq", resReg, "#0");
    emit("movne", resReg, "#1");
    emit("cmpeq", reg2, "#0");
    emit("moveq", resReg, "#0");
}

void Obj::logicOr(string resReg, string reg1, string reg2)
{
    emit("cmp", reg1, "#0");
    emit("moveq", resReg, "#0");
    emit("movne", resReg, "#1");
    emit("cmpeq", reg2, "#0");
    emit("movne", resReg, "#1");
}

void Obj::logicNot(string resReg, string reg1) 
{
    // Pro
    emit("cmp", reg1, "#0");
    emit("moveq", resReg, "#1");
    emit("movne", resReg, "#0");
}

void Obj::cmp(string resReg, string reg1, string reg2, string cmp, string fcmp)
{
    emit("cmp", reg1, reg2);
    emit("mov" + cmp, resReg, "#1");
    emit("mov" + fcmp, resReg, "#0");
}

void Obj::label(string name)
{
    emit(name, ":");
}

void Obj::comment(string str)
{
    emit("#", str);
}

void Obj::nop()
{
    emit("");
}

void Obj::printObjCode(FILE* file)
{
    for(auto i: arm_code)
    {
        string code = i->getArmCode();
        if(code != "") fprintf("file, \t%s\n", code.c_str());
    }
}

void Obj::initVar(string reg, string tmpReg, Var* var)
{
    if(!var->isInitVar()) return;
    if(var->isBaseType())
        ldrImm(reg, var->getVal());
    else 
        ldrFake(reg, var->getPtrVal());
    strVar(reg, tmpReg, var);
}

void Obj::in_out(string reg0, string reg1, string reg2, Var* var, Operator op)
{
    int offset = var->getOffset();
    //printf("I got !"); system("pause");
    if(!offset)
    {
        if(op == OP_READ) ldrFake(reg0, var->getName());
        else ldrVar(reg0, var);
    }
    else
    {
        if(op == OP_READ) leaStack(reg0, offset);
        else ldrVar(reg0, var);
    }   
    //printf("I got in_out1!"); system("pause");
    ldrFake(reg2, SymTab::deciOut->getName());
    //printf("I got in_out2!"); system("pause");
    emit("mov", reg1, reg0);
    if(op == OP_READ) emit("bl", "scanf");
    else emit("bl", "printf");
    //printf("I got in_out3!"); system("pause");
    //Pro 是否需要寄存器保护
}