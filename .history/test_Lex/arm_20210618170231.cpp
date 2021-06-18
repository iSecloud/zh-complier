#include "arm.h"

const char* regName[8] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

void ARM::rotateRight(unsigned int &num)
{
    num = ((num & 3) << 30) | (num >> 2);
}

bool ARM::validConst(unsigned int num)
{
    for(int i = 0; i < 16; i ++)
    {
        if(num <= 0xff) return true;
        rotateRight(num);
    }
}

bool ARM::validConstExpr(int num)
{
    return validConst(num) || validConst(-num);
}