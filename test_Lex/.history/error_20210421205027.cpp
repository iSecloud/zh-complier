#include "error.h"
#include "lex.h"

string errinfo[] = {
        "File Open Error!", "Incomplete reading of file data!",
        "Illegal input charactor", "Illegal input string!", "Illegal comment!"
};
Scanner *Error::sc = NULL;

Error::Error(Scanner *sc1) 
{
    errNum = 0;
    sc = sc1;
}

Error::~Error() {}

void Error::showError(Err errCode)
{
    int posx = sc->getRowCol().first;
    int posy = sc->getRowCol().second;
    printf("Error: the %d row, the %d col: ", posx, posy);
    printf("%s\n", errinfo[(int)errCode]);
}