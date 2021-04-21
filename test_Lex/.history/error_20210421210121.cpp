#include "error.h"
#include "token.h"
#include "lex.h"

string errinfo[] = {
        "File Open Error!", "Incomplete reading of file data!",
        "Illegal input charactor", "Illegal input string!", "Illegal comment!"
};

Scanner *Error::sc = NULL;
int Error::errNum = 0;

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
    printf("Error %d: the %d row, the %d col: ", ++errNum, posx, posy);
    printf("%s\n", errinfo[(int)errCode]);
    cout << errinfo[(int)errCode] << endl;
}