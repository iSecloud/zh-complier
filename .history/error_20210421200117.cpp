#include "error.h"
#include "lex.h"

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

void Error::showError(Err errCode)
{
    int posx = sc.getRowCol().first;
    int posy = sc.getRowCol().second;
    printf("Error: the %d row, the %d col: ", posx, posy);
    printf(errinfo[(int)errCode])
}