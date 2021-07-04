#pragma once
#include "common.h"
using namespace std;

class SymTab
{
    //记录添加顺讯
    vector<string> varList;
    vector<string> funList;

    unordered_map<string, vector<Var*>*> varTab; //变量表
    unordered_map<string, Var*> strTab; //特殊 字符串表
    unordered_map<string, Fun*> funTab; //函数表

    Fun* curFun; //当前函数
    int scopeNum; //全局路径编号
    vector<int> scopePath; //当前路径
    GenIR *ir; //中间代码生成

public:

    //特殊变量
    static Var* voidVar;
    static Var* oneVar;
    static Var* fourVar;
    static Var* zeroVar;
    static Var* deciOut;
    
    //符号表初始化
    SymTab();
    ~SymTab();
    
    //作用域管理
    void enter();
    void leave();
    
    //变量表和函数表管理
    void addVar(Var* var);
    void addStr(Var* &var);
    void addCode(Quaternion *code); //添加一个中间代码
    Var* getVar(string name);
    void decFun(Fun* fun); //声明一个函数
    void defFun(Fun* fun); //定义一个函数
    void endDefFun(); //结束定义
    Fun* getFun(string name, vector<Var* >&paraList); //得到函数

    //外部接口
    vector<int> &getScope();
    vector<string> &getVarList();
    vector<string> &getFunList();
    void setIR(GenIR *ir);
    Fun* getCurFun();
    vector<Var*> getGlbVar();
    void printInterInfo(int isopt);
    void genSegment();
    void genAssemble(bool isopt);
    void genOptmize();
};