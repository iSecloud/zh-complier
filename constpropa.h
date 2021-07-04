#pragma once
#include "common.h"
#include "symtab.h"
#include "dfg.h"
#include "symbol.h"

class ConstPropagation
{
    SymTab* symtab;
    DFG* dfg;
    vector<Var* > vars; //变量集合
    vector<Var* > glbVars; //全局变量集合
    vector<double > boundVals; //边界值集
    vector<double > initVals; //初值值集

public:
    static double UNDEF;
    static double NAC;
    ConstPropagation(SymTab* symtab1, DFG* dfg1, vector<Var* >&paraVar);
    ~ConstPropagation();
    double joinVar(double left, double right);
    void joinBlock(Block* block);
    void transferVar(Quaternion* code, vector<double> &in, vector<double> &out);
    bool transferBlk(Block* block);
    void propagate(); //常量传播框架
    void analysis(); //常量传播方程分析
    void algSimple(); //代数化简
    void jCondSimple(); //消除不可达数据块
};