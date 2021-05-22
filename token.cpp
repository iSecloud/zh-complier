#include <sstream>
#include "token.h"

//-------------------基类-------------------
Token::Token(Tag t) 
{
    tag = t;
}
Token::~Token() {}
string Token::toString() 
{
    return TagName[tag];
}


//-------------------数字-------------------
Num::Num(int x): Token(NUM)
{
    num = x;
}
Num::~Num() {}
string Num::toString()
{
    stringstream ss;
    string s;
    ss << num; ss >> s;
    return TagName[tag] + " " + s;
}

//-------------------字符-------------------
Char::Char(char x): Token(CHAR)
{
    ch = x;
}
Char::~Char() {}
string Char::toString()
{
    return TagName[tag] + " " + ch;
}

//-------------------字符串-------------------
Str::Str(string s): Token(STR)
{
    str = s;
}
Str::~Str() {}
string Str::toString()
{
    return TagName[tag] + " " + str; 
}

//-------------------标识符-------------------
Id::Id(string s): Token(ID)
{
    id = s;
}
Id::~Id() {}
string Id::toString()
{
    return TagName[tag] + " " + id;
}

Keywords::Keywords()
{
    key["int"] = KW_INT;
    key["char"] = KW_CHAR;
    key["void"] = KW_VOID;
    key["extern"] = KW_EXTERN;
    key["if"] = KW_IF;
    key["else"] = KW_ELSE;
    key["switch"] = KW_SWITCH;
    key["case"] = KW_CASE;
    key["default"] = KW_DEFAULT;
    key["for"] = KW_FOR;
    key["do"] = KW_DO;
    key["while"] = KW_WHILE;
    key["continue"] = KW_CONTINUE;
    key["break"] = KW_BREAK;
    key["return"] = KW_RETURN;
    key["Secloud"] = KW_SECLOUD;
    key["read"] = KW_READ;
    key["write"] = KW_WRITE;
}

Tag Keywords::getTag(string name)
{
    return (enum Tag)key[name]; //0表示无此关键词
}