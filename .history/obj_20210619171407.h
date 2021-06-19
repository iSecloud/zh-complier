#pragma once
#include "common.h"

// arm指令
class Arm
{
    bool dead; //死代码
    string op; //操作符
    string res, arg1, arg2; //结果 参数1 参数2
    string addition; //注释
    
public:
    Arm(string op, string res="", string arg1="", string arg2="", string addition="");
    ~Arm();
    //TODO 替换指令
    void setDead();
    string getArmCode();
};

class Obj
{
    vector<Arm* > arm_code;
    string immNum(int num, int f=true); //#18, 18

public:
    Obj();
    ~Obj();
    void objCode(string op);
    void objCode(string op, string res);
    void objCode(string op, string res, string arg1);
    void objCode(string op, string res, string arg1, string arg2);
};