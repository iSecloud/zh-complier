#pragma once
#include "common.h"

class Error
{
    int errNum;
    Scanner &sc;
public:
    Error(Scanner &sc1);
    ~Error();

    void printErrorInfo(pair<int, int> pos, enum Err err);

};