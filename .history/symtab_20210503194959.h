#pragma once
#include "common.h"
#include "symbol.h"
#include<unordered_map>

class SymTab
{
    unordered_map<string, vector<Var*>*> varTab; 
};