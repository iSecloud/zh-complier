#pragma once

// 变量定义
class Var
{
    vector<int > scopePath; //作用域路径
    Tag type; //类型 
    string name; //名字
    bool literal; //是否为常量/字面量
    bool isExtern; //是否为extern声明
    bool isArray; //是否为数组
    int arrayLen; //数组长度
    bool isPtr; //是否为指针
    bool isLeft; //是否为左值
    Var* initVal; //初始值
    bool isInit; //是否初始化
    int intVal; //int常量初值
    char charVal; //char常量初值
    string strVal; //str常量初值
    string ptrVal; //字符指针初值?
    Var* ptr; //指针变量
    int size; //变量大小
    int offset; //变量的偏移量

};

// 函数定义
class Fun
{

};