#pragma once
#include "common.h"

class Token //基类
{
public:
    Tag tag;
    Token(Tag t);
    virtual string toString();
    virtual ~Token();
};

class Num: public Token //数字常量
{
public:
    int num;
    Num(int x);
    virtual string toString();
    virtual ~Num();
};

class Char: public Token //字符常量
{
public:
    char ch;
    Char(char x);
    virtual string toString();
    virtual ~Char();
};

class Str: public Token //字符串常量
{
public:
    string str;
    Str(string x);
    virtual string toString();
    virtual ~Str();
};

class Id: public Token //标识符
{
public:
    string id;
    Id(string x);
    virtual string toString();
    virtual ~Id();
};

class Keywords //关键字
{
public:
    map<string, int> key;
    Keywords();
    Tag getTag(string name);
};