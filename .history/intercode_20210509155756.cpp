#include "intercode.h"
#include "common.h"

void Quaternion::init()
{
    label = "";
    op = OP_NOP;
    result = arg1 = arg2 = NULL;
    fun = NULL;
    target = NULL;   
}

Quaternion::Quaternion()
{}

Quaternion::~Quaternion()
{}

Quaternion::Quaternion(Operator op, Var* result, Var* arg1, Var* arg2)
{
    init();
    this->op = op;
    this->result = result;
    this->arg1 = arg1;
    this->arg2 = arg2;
}

Quaternion::Quaternion(Operator op, Fun* fun, Var* result) //函数相关指令
{
    init();
    this->op = op;
    this->fun = fun;
    this->result = result;
}

Quaternion::Quaternion(Operator op, Var* arg1) //单参或无参指令
{
    init();
    this->op = op;
    this->arg1 = arg1;
}

Quaternion::Quaternion(Operator op) //特殊 产生标签
{
    init();
    this->op = op;
    //TODO 产生label
}

Quaternion::Quaternion(Operator op, Quaternion* target, Var* arg1, Var* arg2) //跳转指令
{
    this->op = op;
    this->target = target;
    this->arg1 = arg1;
    this->arg2 = arg2;
}

InertCode::InterCode()
{
    
}