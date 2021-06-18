#pragma once
#include "common.h"

// arm指令
class Arm
{
    bool dead; //死代码
    string op; //操作符
    string res, arg1, arg2;
    string addition; //注释
    Arm(string op, string res="", string arg1="", string arg2="", string addition="")
    ~Arm();
};