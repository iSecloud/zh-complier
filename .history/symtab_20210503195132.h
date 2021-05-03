#pragma once
#include "common.h"
#include "symbol.h"
using namespace std;

class SymTab
{
    unordered_map<string, vector<Var*>*> varTab; 
};