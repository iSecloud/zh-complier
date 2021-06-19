#pragma once
#include "common.h"
#include "symbol.h"
#include "armplat.h"
#include <cstring>
#include <sstream>

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
    string immNum(int num, bool f=true); //#18, 18

public:
    Obj();
    ~Obj();
    // 普通指令
    void objCode(string op);
    void objCode(string op, string res);
    void objCode(string op, string res, string arg1);
    void objCode(string op, string res, string arg1, string arg2);
    //加载指令ldr
    void ldrImm(string reg, int num); //立即数加载到寄存器
    void ldrVar(string reg, Var* var); //变量加载到寄存器
    void ldrFake(string reg, string name); //伪指令LDR, LDR{cond} Rm, =addr
    void ldrBase(string reg, string baseReg, int offset, bool isChar); //寄存器基址寻址
    void ldrArg(Fun* fun); //加载参数
    //储存指令
    void strBase(string reg, string baseReg, string tmpReg, int offset, bool isChar); 
    void strVar(string reg, string tmpReg, Var* var);
    //函数相关
    void leaStack(string reg, int offset); //从栈中加载地址
    void allocStack(Fun* fun, string tmpReg);
};