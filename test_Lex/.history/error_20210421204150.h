#pragma once
#include "common.h"
#include "lex.h"

class Error
{
    int errNum;
    static Scanner *sc;

public:
    Error(Scanner *sc1);
    ~Error();

    static void showError(Err errCode);
};