#include "intercode.h"
#include "genir.h"
#include "symbol.h"
#include "symtab.h"

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

Operator Quaternion::getOp()
{
    return op;
}

string Quaternion::getLabel()
{
    return label;
}

Fun* Quaternion::getFun()
{
    return fun;
}

Var* Quaternion::getResult()
{
    return result;
}

Var* Quaternion::getArg1()
{
    return arg1;
}

Var* Quaternion::getArg2()
{
    return arg2;
}

void Quaternion::setArg1(Var* arg)
{
    arg1 = arg;
}

Quaternion* Quaternion::getTarget()
{
    return target;
}

void Quaternion::setFirst()
{
    isFirst = true;
}

bool Quaternion::getFirst()
{
    return isFirst;
}

bool Quaternion::isJmp()
{
    if(op == OP_JMP || op == OP_RET || op == OP_RETX)
        return true;
    return false;
}

bool Quaternion::isJmpCond()
{
    if(op == OP_JF || op == OP_JT || op == OP_JNE)
        return true;
    return false;
}

bool Quaternion::isDec()
{
    return op == OP_DEC;
}

Block* Quaternion::getBlock()
{
    return block;
}

void Quaternion::setBlock(Block* blk)
{   
    block = blk;
}

void Quaternion::replace(Operator op, Quaternion* target, Var* arg1, Var* arg2)
{
    this->op = op;
    this->target = target;
    this->arg1 = arg1;
    this->arg2 = arg2;
}

void Quaternion::replace(Operator op, Var* result, Var* arg1, Var* arg2)
{
    this->op = op;
    this->result = result;
    this->arg1 = arg1;
    this->arg2 = arg2;
}

void Quaternion::toString()
{
    //注意 string用%s输出时要用内置的c_str()转成char*来输出

    if(op == OP_LABEL) printf("OP_LABEL %s", label.c_str());
    else if(op == OP_ENTRY) printf("OP_ENTRY %s", fun->getName().c_str());
    else if(op == OP_EXIT) printf("OP_EXIT %s", fun->getName().c_str());
    else if(op == OP_DEC) printf("OP_DEC var is "), arg1->info();
    else if(op == OP_ASSIGN) printf("OP_ASSIGN "), result->info(), printf("="), arg1->info();
    else if(op == OP_ADD) printf("OP_ADD "), result->info(), printf("="), arg1->info(), printf("+"), arg2->info();
    else if(op == OP_SUB) printf("OP_SUB "), result->info(), printf("="), arg1->info(), printf("-"), arg2->info();
    else if(op == OP_MUL) printf("OP_MUL "), result->info(), printf("="), arg1->info(), printf("*"), arg2->info();
    else if(op == OP_DIV) printf("OP_DIV "), result->info(), printf("="), arg1->info(), printf("/"), arg2->info();
    else if(op == OP_MOD) printf("OP_MOD "), result->info(), printf("="), arg1->info(), printf("mod"), arg2->info();
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
    else if(op == OP_SET) printf("OP_SET "), printf("*"), arg1->info(), printf("="), result->info();
    else if(op == OP_GET) printf("OP_GET "), result->info(), printf("="), printf("*"), arg1->info();
    else if(op == OP_JMP) printf("OP_JMP "), printf("goto %s", target->label.c_str());
    else if(op == OP_JNE) printf("OP_JNE "), printf("if "), arg1->info(), printf("!="), arg2->info(), printf(" then goto %s", target->label.c_str());
    else if(op == OP_JT) printf("OP_JT "), printf("if "), arg1->info(), printf(" then goto %s", target->label.c_str());
    else if(op == OP_JF) printf("OP_JF "), printf("if not "), arg1->info(), printf(" then goto %s", target->label.c_str());
    else if(op == OP_ARG) printf("OP_ARG push arg "), arg1->info();
    else if(op == OP_PROC) printf("OP_PROC call function %s", fun->getName().c_str());
    else if(op == OP_CALL) printf("OP_CALL "), result->info(), printf("=%s", fun->getName().c_str());
    else if(op == OP_RET) printf("OP_RET return point is %s", target->label.c_str());
    else if(op == OP_RETX) printf("OP_RETX return value is "), arg1->info(), printf(" and the return point is %s", target->label.c_str());
    else if(op == OP_READ) printf("OP_READ read the "), arg1->info();
    else if(op == OP_WRITE) printf("OP_WRITE write the "), arg1->info();

    printf("\n");
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

vector<Quaternion*>& InterCode::getInterCode()
{
    return intercode;
}

void InterCode::makeFirst()
{
    intercode[0]->setFirst(); //Enter
    intercode[1]->setFirst();  //第一条
    intercode[intercode.size() - 1]->setFirst(); //Exit
    for(int i = 1; i < intercode.size(); i ++)
    {
        Quaternion* code = intercode[i];
        if(code->isJmp() || code->isJmpCond())
        {
            code->getTarget()->setFirst(); //标签是首指令
            intercode[i + 1]->setFirst(); //跳转指令下一条是首指令
        }
    }
}