#include "symbol.h"

void Fun::enterScope()
{
    scopeEsp.push_back(0); //声明一个新的作用域
}

void Fun::leaveScope()
{
    curEsp -= scopeEsp.back();
    scopeEsp.pop_back();
}