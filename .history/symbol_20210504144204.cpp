#include "symbol.h"
#include "error.h"

void Var::setExtern(bool ext)
{
    isExtern = ext;
    if(ext) size = 0; //声明不占空间？暂定...
}

void Var::setPtr(bool ptr)
{
    isPtr = ptr;
    if(ptr && !isExtern) size = 4; //指针大小四字节
}

void Var::setType(Tag t)
{
    if(t == KW_VOID)
    {
        Error::showError(TYPE_DEF_ERR);
        //不能声明为void型
    }
    else
    {
        type = t;
        if(type == KW_INT && !isExtern) size = 4;
        if(type == KW_CHAR && !isExtern) size = 1;
    }
}

void Var::setName(string sname)
{
    //TODO 处理空name
    name = sname;
}

void Var::setArray(int len)
{
    if(len <= 0)
    {
        Error::showError(ARRAY_LEN_ERR);
        //数组长度必须大于0
    }
    else
    {
        isArray = true;
        arrayLen = len;
        isLeft = false;
        if(!isExtern) size = size * arrayLen;
    }
}

void Var::clear()
{
    scopePath.clear();
    type = KW_INT; //默认为int
    
    literal = false;
    isExtern = false;
    isArray = false; 
    arrayLen = 0; 
    isPtr = false;
    isLeft = true; //默认为左值
    initVal = NULL; 
    isInit = false;
    ptr = NULL;
    size = offset = 0;
}

// 变量 指针的构造函数
Var::Var(vector<int> &scp, bool ext, Tag t, bool ptr, string name, Var* init = NULL)
{
    clear();
    scopePath = scp;
    setExtern(ext);
    setType(t);
    setPtr(ptr);
    setName(name);
    initVal = init;
}

//数组的构造函数
Var::Var(vector<int> &scp, bool ext, Tag t, string name, int len)
{
    clear();
    scopePath = scp;
    setExtern(ext);
    setType(t);
    setName(name);
    setArray(len);
}

Var::Var(Token* it)
{
    clear();
    literal = true; //这是一个常量
    isLeft = false; // 常量不能作为左值
    if(it->tag == NUM)
    {
        setType(KW_INT);
        name = "<int>";
        intVal = ((Num*)it)->num; //设置初值
    }
    else if(it->tag == CHAR)
    {
        setType(KW_CHAR);
        name = "<char>";
        charVal = ((Char*)it)->ch;
    }
    else 
    {
        //字符串以字符数组存下来
        setType(KW_CHAR);
        //TODO 创建一个名字
        strVal = ((Str*)it)->str;
        
    }
}

void Fun::enterScope()
{
    scopeEsp.push_back(0); //声明一个新的作用域
}

void Fun::leaveScope()
{
    maxDep = max(maxDep, curEsp);
    curEsp -= scopeEsp.back(); //离开作用域时，更新当前栈深
    scopeEsp.pop_back(); 
}