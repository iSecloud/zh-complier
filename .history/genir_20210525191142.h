#pragma once
#include "common.h"

class GenIR //生成中间代码
{
    SymTab &symtab;
    static int labelNum;

public:
    GenIR(SymTab &symtab1);
    ~GenIR();

    bool typeCheck(Var* lval, Var* rval); //类型匹配
    string genLb();

    void genFunHead(Fun* fun); //产生函数入口
    void genFunTail(Fun* fun); //产生函数出口
    void genFunReturn(Var* ret); //产生return语句
    void genPara(Var* arg); //传递参数
    Var* genCall(Fun* fun, vector<Var*>& args); //调用函数
    void genIfHead(Var* condition, Quaternion*& _else);
    void genElseHead(Quaternion*& _else, Quaternion*& _exit);
    void genElseTail(Quaternion*& _exit);
    void genIfTail(Quaternion*& _else);
    void genSwitchHead(Quaternion*& _exit);
    void genCaseHead(Var* condition, Var* lit, Quaternion*& _case);
    void genCaseTail(Quaternion*& _case);
    void genSwitchTail(Quaternion*& _exit);
    void genWhileHead(Quaternion*& _while, Quaternion*& _exit);
    void genWhilecond(Var* condition, Quaternion*& _exit);
    void genWhileTail(Quaternion*& _while, Quaternion*& _exit);
    void genDoWhileHead(Quaternion*& _do, Quaternion*& _exit);
    void genDoWhileTail(Quaternion*& _do, Quaternion*& _exit, Var* condition);
    void genForHead(Quaternion*& _for, Quaternion*& _exit, Quaternion*& _block);
    void genForStep(Quaternion*& _exit, Quaternion*& _block, Quaternion*&step, Var* condition);
    void genForBlockHead(Quaternion*& _for, Quaternion*& _block);
    void genForBlockTail(Quaternion*& _step, Quaternion*& _exit);   

    Var* genVal(Var* var); //拷贝(取值)
    Var* genBinOp(Tag opt, Var* lval, Var* rval); //双目运算
    Var* genAssign(Var* lval, Var* rval); //赋值运算
    Var* genAdd(Var* lval, Var* rval); // +
    Var* genSub(Var* lval, Var* rval); // -
    Var* genMul(Var* lval, Var* rval); // *
    Var* genAand(Var* lval, Var* rval); //逻辑&&
    Var* genOor(Var* lval, Var* rval); //逻辑||
    Var* genEqu(Var* lval, Var* rval); // ==
    Var* genNequ(Var* lval, Var* rval); // !=
    Var* genDiv(Var* lval, Var* rval); // /
    Var* genMod(Var* lval, Var* rval); // %
    Var* genAnd(Var* lval, Var* rval); // & 
    Var* genOr(Var* lval, Var* rval); // |
    Var* genXor(Var* lval, Var* rval); // ^
    Var* genGe(Var* lval, Var* rval); // >=
    Var* genGt(Var* lval, Var* rval); // > 
    Var* genLe(Var* lval, Var* rval); // <= 
    Var* genLt(Var* lval, Var* rval); // < 
    Var* genSigOpLeft(Tag opt, Var* var); //左单目运算
    Var* genPtr(Var* var); //指针运算*p
    Var* genAddress(Var* var); //取值运算&p
    Var* genIncLeft(Var* var); // ++
    Var* genDecLeft(Var* var); // --
    Var* genNeg(Var* var); // -
    Var* genNot(Var* var); // !
    Var* genInv(Var* var); // ~
    Var* genSigOpRight(Tag opt, Var* var); //右单目运算
    Var* genIncRight(Var* var); // ++
    Var* genDecRight(Var* var); // --
    Var* genArray(Var* arr, Var* index); //数组运算

};