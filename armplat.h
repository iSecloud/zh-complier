#pragma once
#include "common.h"
#include <sstream>

/*
这里定义了一些ARM平台的规则
-----------------------------寄存器信息-----------------------------------
可使用的通用寄存器的个数r0-r7
"r8" 用于加载操作数1,保存表达式结果
"r9" 用于加载操作数2,写回表达式结果,立即数，标签地址
"r10" 用于保存乘法结果，虽然mul r8,r8,r9也能正常执行，但是避免交叉编译提示错误！
"fp" r11,局部变量寻址
"ip" "r12"，临时寄存器
"sp" r13，栈指针
"lr" r14，链接寄存器
"pc" r15，程序计数器

fp, sp是管理栈空间的, 
lr是保存跳转之前的返回地址, 
pc是程序计数器, 
ip内部过程调用寄存器

----------------------------常数表达式(第二操作数)------------------------------
	判断num是否是常数表达式，8位数字循环右移偶数位得到
	合法的常量表达式是一个无符号32位数值，其规范是：一个8位的无符号数值常量，
	其余位用0填充到32位以后，循环右移偶数次后得到的32位数值

----------------------------立即数----------------------------------------------
在偏移量中， 立即数绝对值不大于2^12的数值，可以使用带符号数

-----------------------------栈帧保护-------------------------------------------
进栈指令序列:
    mov ip,sp
    stmfd sp!,{fp,ip,lr,pc}
    sub fp,ip,#4 //sub fp,ip,#4使fp指向当前函数栈帧的栈底
出栈指令序列：
    ldmea fp,{fp,sp,pc}
栈基址fp指向pc，继续入栈需要在偏移12字节基础之上！

*/
class ARM
{
    static void rotateRight(unsigned int& num); //循环右移两位
    static bool validConst(unsigned int num); //是否是合法立即数
public:
    static bool validConstExpr(int num);
    static bool validOffset(int num);
    static bool validReg(string reg);
    static const int regNum = 8; //r0-r7
    static const char* regName[8];
    static const int stackBase = 12; //栈基址
    static const int stackBasePro = 44; //12+4*8 受保护的栈基址
};