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
    else if(op == OP_ASSIGN) printf("OP_ASSIGN "), result->info(), printf("="), arg1->info();
    else if(op == OP_ADD) printf("OP_ADD "), result->info(), printf("="), arg1->info(), printf("+"), arg2->info();
    else if(op == OP_SUB) printf("OP_SUB "), result->info(), printf("="), arg1->info(), printf("-"), arg2->info();
    else if(op == OP_MUL) printf("OP_MUL "), result->info(), printf("="), arg1->info(), printf("*"), arg2->info();
    else if(op == OP_DIV) printf("OP_DIV "), result->info(), printf("="), arg1->info(), printf("/"), arg2->info();
    else if(op == OP_MOD) printf("OP_MOD "), result->info(), printf("="), arg1->info(), printf("%"), arg2->info();
    else if(op == OP_NEG) printf("OP_NEG "), result->info(), printf("="), printf("-"), arg1->info();
    else if(op == OP_AND) printf("OP_AND "), result->info(), printf("="), arg1->info(), printf("&"), arg2->info();
    else if(op == OP_OR) printf("OP_OR "), result->info(), printf("="), arg1->info(), printf("|"), arg2->info();
    else if(op == OP_XOR) printf("OP_XOR "), result->info(), printf("="), arg1->info(), printf("^"), arg2->info();
    else if(op == OP_INV) printf("OP_INV "), result->info(), printf("="), printf("~"), arg1->info();
    else if(op == OP_AAND) printf("OP_AADD "), result->info(), printf("="), arg1->info(), printf("&&"), arg2->info();
    else if(op == OP_OOR) printf("OP_OOR "), result->info(), printf("="), arg1->info(), printf("||"), arg2->info();
    else if(op == OP_GE) printf("OP_GE "), result->info(), printf("="), arg1->info(), printf(">="), arg2->info();
    else if(op == OP_GT) printf("OP_GT "), result->info(), printf("="), arg1->info(), printf(">"), arg2->info();
    else if(op == OP_LE) printf("OP_LE "), result->info(), printf("="), arg1->info(), printf("<="), arg2->info();
    else if(op == OP_LT) printf("OP_LT "), result->info(), printf("="), arg1->info(), printf("<"), arg2->info();
    else if(op == OP_EQU) printf("OP_EQU "), result->info(), printf("="), arg1->info(), printf("=="), arg2->info();
    else if(op == OP_NEQU) printf("OP_NEQU "), result->info(), printf("="), arg1->info(), printf("!="), arg2->info();
    else if(op == OP_NOT) printf("OP_NOT "), result->info(), printf("="), printf("!"), arg1->info();
    else if(op == OP_LEA) printf("OP_LEA "), result->info(), printf("="), printf("&"), arg1->info();
    else if(op == OP_SET) printf("OP_SET "), printf("*"), arg2->info(), printf("="), arg1->info();
    else if(op == OP_GET) printf("OP_GET "), result->info(), printf("="), printf("*"), arg1->info();
    else if(op == OP_JMP) printf("OP_JMP "), printf("goto %s", target->label);
    else if(op == OP_JNE) printf("OP_JNE "), printf("if "), arg1->info(), printf("!="), arg2->info(), printf(" then goto %s", target->label);
    else if(op == OP_JT) printf("OP_JT "), printf("if "), arg1->info(), printf(" then goto %s", target->label);
    else if(op == OP_JF) printf("OP_JF "), printf("if not "), arg1->info(), printf(" then goto %s", target->label);
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

