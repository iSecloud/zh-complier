#pragma once
#include <string.h>
#include <map>
#include <iostream>
using namespace std;

enum Tag //词法记号
{
    ERR, END, //错误 结束符    
    KW_SECLOUD, //自定义关键字
    ID, //标识符
    NUM, CHAR, STR, //常量：数字 字符 字符串
    KW_INT, KW_CHAR, KW_VOID, KW_EXTERN, //基本类型
    KW_READ, KW_WRITE, //输入输出
    KW_IF, KW_ELSE, KW_SWITCH, KW_CASE, KW_DEFAULT, //条件判断
    KW_FOR, KW_DO, KW_WHILE, //循环
    KW_CONTINUE, KW_BREAK, KW_RETURN,//跳出语句
    ADD, SUB, MUL, DIV, MOD, INC, DEC, //整数运算符+ - * / % ++ --
    NOT, AND, OR, XOR, //位运算符 ~ & | ^
    NNOT, AAND, OOR, //双目运算符 ! || &&
    ASSIGN, //赋值运算符 =
    GT, GE, LT, LE, EQU, NEQU, //关系运算符 > >= < <= == !=
    COMMA, COLON, SEMICON, //逗号, 冒号: 分号;
    LPAREN, RPAREN, LBRACKET, RBRACKET, LBRACE, RBRACE //小括号()  中括号[]  大括号{}
};

enum Err //错误记号
{
    //文件相关
    FILE_OPEN_ERROR, FILE_CLOSE_ERROR, 

    //字符相关
    ILLEGAL_CHARACTER, ILLEGAL_STRING, ILLEGAL_COMMENT,
    
    //语法相关
    TYPE_LOST, IDNAME_LOST, RPAREN_LOST, NUM_LOST, RBRACKET_LOST, COMMA_LOST,
    SEMICON_LOST, LBRACE_LOST
};