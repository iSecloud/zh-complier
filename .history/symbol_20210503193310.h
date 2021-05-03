#pragma once

// 变量定义
class Var
{
    vector<int > scopePath; //作用域路径
    Tag type;
    string name;
    bool literal; 
    bool isExtern;
    bool isArray;
    int arrayLen;
    bool isPtr;

};

// 函数定义
class Fun
{

};