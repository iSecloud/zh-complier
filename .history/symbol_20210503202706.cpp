#include "symbol.h"

void Var::clear()
{
    scopePath.clear();
    type = KW_INT; //默认为int
    
    literal = false;
    isExtern = false;
    isArray = false; //是否为数组
    arrayLen = 0; //数组长度
    isPtr = false;; //是否为指针
    isLeft = true; //是否为左值
    initVal = NULL; //初始值
    isInit = false;; //是否初始化

}

void Fun::enterScope()
{
    scopeEsp.push_back(0); //声明一个新的作用域
}

void Fun::leaveScope()
{
    maxDep = max(maxDep, curEsp);
    curEsp -= scopeEsp.back(); //离开作用域时，更新当前栈深
    scopeEsp.pop_back(); 
}