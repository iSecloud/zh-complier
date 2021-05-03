#pragma once
#include "common.h"
#include "symbol.h"
#include<unordered_map>
using namespace std;

class SymTab
{
    unordered_map<string, vector<Var*>*> varTab; 
};