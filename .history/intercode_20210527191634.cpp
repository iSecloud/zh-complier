#include "intercode.h"

void Quaternion::init()
{
    label = "";
    op = OP_NOP;
    result = arg1 = arg2 = NULL;
    fun = NULL;
    target = NULL;   
}

Quaternion::Quaternion()
{
    init();
    this->op = OP_LABEL;
    label = GenIR::genLb();
}

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

Quaternion::Quaternion(Operator op, Quaternion* target, Var* arg1, Var* arg2) //跳转指令
{
    this->op = op;
    this->target = target;
    this->arg1 = arg1;
    this->arg2 = arg2;
}

void Quaternion::toString()
{
    if(op == OP_LABEL) printf("OP_LABEL %s", label);
    else if(op == OP_ENTRY) printf("OP_ENTRY %s", fun->getName());
    else if(op == OP_EXIT) printf("OP_EXIT %s", fun->getName());
    else if(op == OP_DEC) printf("OP_DEC var is "), arg1->info();
    else if(op == OP_ASSIGN) printf("OP_ASSIGN")

}

InterCode::InterCode()
{
    intercode.clear();
}

InterCode::~InterCode()
{
    for(auto i: intercode)
        delete i;
}

void InterCode::addQuaternion(Quaternion* code)
{
    intercode.push_back(code);
}

