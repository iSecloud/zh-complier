#pragma once
#include "common.h"

class Quaternion //四元式定义
{
    string label;
    Operator op;
    Var *result, *arg1, *arg2;
    Fun *fun;
    Quaternion *target; //跳转标号

    void init(); //初始化 maybe需要

public:
    Quaternion(); //特殊 产生标签
    ~Quaternion();
    Quaternion(Operator op, Var* result, Var* arg1, Var* arg2 = NULL); //一般的运算指令
    Quaternion(Operator op, Fun* fun, Var* result = NULL); //函数相关指令
    Quaternion(Operator op, Var* arg1 = NULL); //单参或无参指令
    Quaternion(Operator op, Quaternion* target, Var* arg1 = NULL, Var* arg2 = NULL); //跳转指令

    Operator getOp();
    string getLabel();
    Fun* getFun();
    Var* getResult();
    Var* getArg1();
    Var* getArg2();
    Quaternion* getTarget();

    void toString(); //输出指令信息
};

class InterCode //中间代码
{

public:
    vector<Quaternion*> intercode;
    InterCode();
    ~InterCode();
    vector<Quaternion*>& getInterCode();
    void addQuaternion(Quaternion* code); //添加代码
};