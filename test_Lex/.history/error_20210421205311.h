#pragma once
#include "common.h"
#include "lex.h"

class Error
{
    static int errNum;
    static Scanner *sc;

public:
    Error(Scanner *sc);
    ~Error();

    static void showError(Err errCode);
};