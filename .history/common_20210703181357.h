#pragma once
#include <string.h>
#include <map>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <ctime>
#include <cmath>
#include <list>
using namespace std;

static string TagName[] = {
    "error", "end",
    "Secloud",
    "id",
    "Num", "Char", "String",
    "int", "char", "void", "extern", 
    "read", "write",
    "if", "else", "switch", "case", "default",
    "for", "do", "while",
    "continue", "break", "return",
    "+", "-", "*", "/", "%", "++", "--",
    "~", "&", "|", "^",
    "!", "&&", "||",
    "=",
    ">", ">=", "<", "<=", "==", "!=",
    ",", ":", ";",
    "(", ")", "[", "]", "{", "}"
};

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
    FILE_OPEN_ERROR, 
    FILE_CLOSE_ERROR, 

    //字符相关
    ILLEGAL_CHARACTER, 
    ILLEGAL_STRING, 
    ILLEGAL_COMMENT,
    
    //语法相关
    TYPE_LOST, 
    IDNAME_LOST, 
    LPAREN_LOST,
    RPAREN_LOST, 
    NUM_LOST, 
    RBRACKET_LOST, 
    COMMA_LOST,
    SEMICON_LOST, 
    LBRACE_LOST, 
    RBRACE_LOST,
    WHILE_LOST,
    COLON_LOST,
    LITERAL_LOST, 

    //语义相关
    ARRAY_LEN_ERR,
    TYPE_DEF_ERR,
    VAR_DEF_MUL,
    VAR_DEF_NONE,
    FUN_DEC_MUL,
    FUN_RET_ERR,
    FUN_DEC_ERR,
    FUN_EXT_ERR, //声明的函数不能被定义
    FUN_DEF_MUL,
    FUN_CALL_ERR,
    FUN_UN_DEC,
    PTR_IS_ERR,
    GET_ADDRESS_ERR,
    NOT_LVAL_ERR,
    TYPE_MATCH_ERR,
    CALC_VAL_ERR,
    ARR_IDX_ERR,
    BREAK_IS_ERR,
    CONTINUE_IS_ERR,
    WRITE_TYPE_ERR,
    INIT_IS_ERR,
    GLB_INIT_ERR,
    READ_TYPE_ERR
};

enum Operator{
    OP_NOP, //空指令
    OP_LABEL, //创建一个标签
    OP_ENTRY, OP_EXIT, //函数入口和出口
    OP_DEC, //声明
    OP_ASSIGN, //赋值
    OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD, //算术运算 + - * / %
    OP_NEG, OP_NOT, OP_INV, OP_INCV, OP_DECV, //单目运算- ! ~ ++ --
    OP_AND, OP_XOR, OP_OR, //位运算& | ^
    OP_AAND, OP_OOR,  //逻辑运算 && ||
    OP_GE, OP_GT, OP_LE, OP_LT, OP_EQU, OP_NEQU, //比较运算 >= > <= < == !=
    OP_LEA, //取址运算
    OP_SET, OP_GET, //设置左值*y = x 和取值x = *y
    OP_JMP, OP_JT, OP_JF, OP_JNE, //跳转指令
    OP_PROC, OP_CALL, //调用指令
    OP_ARG, //传入参数
    OP_RET, OP_RETX, //return指令
    OP_READ, OP_WRITE //读写指令
};

class Token;
class Scanner;
class Error;
class Lexer;
class SymTab;
class GenIR;
class Var;
class Fun;
class Quaternion;
class InterCode;
class Parser;
class Block;
class DFG;