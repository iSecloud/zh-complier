#pragma once

#include "common.h"
#include "intercode.h"

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
    string ptrVal; //字符指针初值 数据段索引
    Var* ptr; //指针变量

    int size; //变量大小
    int offset; //变量的偏移量

private:
    //变量属性的一些定义(私有函数)
    void setExtern(bool ext);
    void setType(Tag t);
    void setPtr(bool ptr);
    void setName(string name);
    void setArray(int len);

public:

    //构造函数
    Var(vector<int> &scp, Var* var);
    Var(vector<int> &scp, Tag t, bool ptr); //临时变量
    Var(vector<int> &scp, bool ext, Tag t, bool ptr, string name, Var* init = NULL); //变量初始化
    Var(vector<int> &scp, bool ext, Tag t, string name, int len); //数组初始化
    Var(Token *tk); //常量初始化
    Var();
    ~Var();
    void clear(); //定义初始化

    //外部接口
    static Var* getStep(Var* var);   
    static Var* getTrue();
    static Var* getFalse();
    static Var* getVoid();  
    void toStringVar();
    bool setInit(); 
    void setOffset(int off);
    void setLeft(bool left);
    void setPointer(Var* nowptr); //设置ptr具体的值是多少
    string getName();
    vector<int > &getScope();
    string getStr();
    Tag getType();
    int getSize();
    bool isBaseType();
    bool isChar();
    bool isConst();
    bool isArr();
    bool isInitVar();
    bool getLeft();
    string getPtrVal();
    int getOffset();
    Var* getPointer();
    Var* getInit();
    int getVal();
    string getRawStr();
    void info(); //输出信息, 方便调试
    //数据流分析
    int index; //列表索引
};

// 函数定义
class Fun
{
    bool isExtern; //是否为extern声明
    Tag type; //类型
    string name; //名字
    vector<Var* > paraVar; //形参列表   
    int maxDep; //最大深度, 用于记录开辟的最大空间
    int curEsp; //当前栈帧位置，即当前栈帧的大小
    vector<int > scopeEsp; //作用域栈帧位置，记录当前作用域的大小
    InterCode incode; //中间代码
    list<Quaternion*> optincode; //优化后的中间代码
    Quaternion *returnPoint; //跳转返回点
    DFG *dfg;

public:
    //构造函数
    Fun();
    ~Fun();
    Fun(bool ext, Tag t, string name1, vector<Var* > &paraList);

    bool matchFun(Fun* fun);
    void define(Fun* fun); //获取参数定义信息？

    //作用域
    void enterScope();
    void leaveScope();
    void locate(Var* var);

    //中间代码相关
    void addCode(Quaternion *code);
    void setReturnPoint(Quaternion *point);
    Quaternion* getReturnPoint();

    //外部接口
    void toStringFun();
    void setExtern(bool ext);
    string getName();
    vector<Var*> &getPara();
    Tag getType();
    bool getExtern();
    int getMaxDep();
    void printInterCode(int isopt);
    void getObjCode(bool isopt);    
    void optimize(SymTab* symtab);
};