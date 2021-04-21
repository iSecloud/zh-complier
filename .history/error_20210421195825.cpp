#include "error.h"

string errinfo[] = {
        "File Open Error!", "Incomplete reading of file data!",
        "Illegal input charactor", "Illegal input string!", "Illegal comment!"
};

Error::Error(Scanner &sc1): sc(sc1) 
{errNum = 0;}

Error::~Error() {}

void Error::printErrorInfo(pair<int, int> pos, enum Err err)
{
    cout << "Error:" << pos.first << "row, " << pos.second << "col: " << errinfo[(int)err];
}