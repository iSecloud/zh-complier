#pragma once
#include "common.h"
#include "symbol.h"

class Quaternion //四元式定义
{
    string label;
    Operator op;
    Var* result, arg1, arg2;
    Fun* fun;
    Quaternion *target; //跳转标号
}