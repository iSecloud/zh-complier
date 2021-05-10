#include "genir.h"
#include "common.h"

int GenIR::labelNum = 0;

GenIR::GenIR(SymTab &symtab1): symtab(symtab1)
{
    labelNum = 0;
}

GenIR::~GenIR()
{}

void GenIR::genFunHead(Fun *fun)
{
    fun->enterScope();  
    symtab.addCode(new Quaternion(OP_ENTRY, fun));
}