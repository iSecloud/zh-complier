#pragma once
#include "common.h"

class Error
{
    int errNum;
public:
    Error();
    ~Error();

    void printErrorInfo(pair<int, int> pos, enum Err err);

};