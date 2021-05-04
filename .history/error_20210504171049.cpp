#include "error.h"
#include "token.h"
#include "lex.h"

string errinfo[] = {
        // 文件相关
        "File Open Error", 
        "Incomplete reading of file data",

        //字符相关
        "Illegal input charactor", 
        "Illegal input string", 
        "Illegal comment",

        //语法相关
        "Missing type definition", 
        "Missing variable name", 
        "Missing \'(\'",
        "Missing \')\'",
        "Missing num", 
        "Missing \']\'", 
        "Missing \',\'", 
        "Missing \';\'",
        "Missing \'{\'", 
        "Missing \'}\'",
        "Missing keyword \'while\'",
        "Missing \':\'",
        "Missing literal",

        //语义相关
        "Illegal array length",
        "Illegal type definition",
        "Variable redefinition",
        "Variable undefined",
        "Duplicate function declaration",
        "Incorrect function return value",
        "Function declaration error"

};

Scanner *Error::sc = NULL;
int Error::errNum = 0;

Error::Error(Scanner *sc1) 
{
    errNum = 0;
    sc = sc1;
}

Error::~Error() 
{}

void Error::showError(Err errCode)
{
    int posx = sc->getRowCol().first;
    int posy = sc->getRowCol().second;
    printf("Error %d: the %d row, the %d col: ", ++errNum, posx, posy);
    cout << errinfo[(int)errCode] << endl;
}
