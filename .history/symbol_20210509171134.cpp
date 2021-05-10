#include "symbol.h"

// ================================变量==============================
void Var::setOffset(off)
{
    offset = off;
}

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

Var::Var()
{}

Var::~Var()
{}

// 变量 指针的构造函数
Var::Var(vector<int> &scp, bool ext, Tag t, bool ptr, string name, Var* init)
{
    clear();
    scopePath = scp;
    setExtern(ext);
    setType(t);
    setPtr(ptr);
    //cout << name << endl;
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

//常量的构造函数
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
        //TODO name未定 创建一个名字 后面代码生成需要根据名字来确定字符串的其实地址
        strVal = ((Str*)it)->str;
        setArray(strVal.size() + 1);
    }
}

string Var::getName()
{
    return name;
}

vector<int >& Var::getScope()
{
    return scopePath;
}

string Var::getStr()
{
    return strVal;
}

Tag Var::getType()
{
    return type;
}

// ================================函数==============================
Fun::Fun()
{}

Fun::~Fun()
{}

Fun::Fun(bool ext, Tag t, string name1, vector<Var* >&paraList)
{
    isExtern = ext;
    type = t;
    name = name1;
    paraVar = paraList;
    curEsp = maxDep = 0; //记录栈深
    for(int i = 0, off = 4; i < paraVar.size(); i ++, off += 4)
        paraVar[i].
    //TODO 计算参数相对于栈帧基址的偏移值
}

bool Fun::matchFun(Fun* fun)
{
    if(fun->getName() != name) return false;
    if(fun->getPara().size() != paraVar.size()) return false;
    vector<Var*> &list = fun->getPara();
    for(int i = 0; i < paraVar.size(); i ++)
        if(list[i]->getType() != paraVar[i]->getType()) //TODO 要不要考虑类型兼容
        {
            Error::showError(FUN_DEC_MUL); 
            return false;
        }
    if(type != fun->getType()) 
    {
        Error::showError(FUN_RET_ERR);
        return false;
    }
    return true;
}

void Fun::define(Fun* fun)
{
    isExtern = false;
    paraVar = fun->getPara();
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

void Fun::setExtern(bool ext)
{
    isExtern = ext;
}

string Fun::getName()
{
    return name;
}

vector<Var* > &Fun::getPara()
{
    return paraVar;
}

bool Fun::getExtern()
{
    return isExtern;
}

Tag Fun::getType()
{
    return type;
}