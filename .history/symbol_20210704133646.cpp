#include "symbol.h"
#include "symtab.h"
#include "error.h"
#include "token.h"
#include "common.h"
#include "genir.h"
#include "armplat.h"
#include "obj.h"
#include "translate.h"
#include "dfg.h"
#include "constpropa.h"

// ================================变量==============================
void Var::toStringVar()
{   
    cout << "Var:\n";
    cout << "Name: " + name << endl;
    cout << "Type: " + TagName[type] << endl;
    for(auto i: scopePath) cout << i << " ";
    printf("\n----------------------\n");
}

Var* Var::getStep(Var* var)
{
    if(var->isBaseType()) return SymTab::oneVar;
    if(var->type == KW_INT) return SymTab::fourVar;
    if(var->type == KW_CHAR) return SymTab::oneVar;
    return NULL;
}

Var* Var::getTrue()
{
    return SymTab::oneVar;
}

Var* Var::getFalse()
{
    return SymTab::zeroVar;
}

Var* Var::getVoid()
{
    return SymTab::voidVar;
}

bool Var::isChar()
{
    return (type == KW_CHAR) && isBaseType();
}

bool Var::isConst()
{
    return literal;
}

bool Var::isBaseType()
{
    if(isPtr || isArray) return false;
    return true;
}

void Var::setOffset(int off)
{
    offset = off;
}

void Var::setLeft(bool left)
{
    isLeft = left;
}

void Var::setPointer(Var* nowptr)
{
    ptr = nowptr;
}

int Var::getSize()
{
    return size;
}

string Var::getName()
{
    return name;
}

vector<int >& Var::getScope()
{
    return scopePath;
}

Var* Var::getInit()
{
    return initVal;
}

bool Var::getLeft()
{
    return isLeft;
}

bool Var::isArr()
{
    return isArray;
}

bool Var::isInitVar()
{
    return isInit;
}

string Var::getStr()
{
    return strVal;
}

string Var::getPtrVal()
{
    return ptrVal;
}

Tag Var::getType()
{
    return type;
}

Var* Var::getPointer()
{
    return ptr;
}

int Var::getVal()
{
    return intVal;
}

int Var::getOffset()
{
    return offset;
}

string Var::getRawStr()
{
    string s = "";
    for(int i = 0; i < strVal.size(); i ++)
    {
        if(strVal[i] == '\n') s.append("\\n");
        else if(strVal[i] == '\t') s.append("\\t");
        else if(strVal[i] == '\\') s.append("\\\\");
        else if(strVal[i] == '\"') s.append("\\\"");
        else if(strVal[i] == '\0') s.append("\\000");
        else s.push_back(strVal[i]);
    }
    s.append("\\000"); //end符
    return s;
}

void Var::info()
{
    if(literal)
    {
        if(type == KW_INT) printf("%d", intVal);
        else if(type == KW_CHAR)
        {
            if(isArray) printf("\"%s\"", name.c_str());
            else printf("\'%c\'", charVal);
        }
    }
    else printf("%s", name.c_str());
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
    if(sname == "") sname = GenIR::genLb(); //生成全局的一个唯一名字
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

bool Var::setInit()
{
    Var* data = initVal;
    if(data == NULL) return false;
    isInit = false;
    if(isExtern) 
        Error::showError(INIT_IS_ERR);
    else if(!GenIR::typeCheck(this, data))
        Error::showError(INIT_IS_ERR);
    else if(data->literal)
    {
        isInit = true;
        if(data->isArray) 
            ptrVal = data->name; //将字符串的名字赋给指针变量
        else
            intVal = data->intVal; //直接将int类型赋给int变量(char类型也可以)
    }
    else
    {
        if(scopePath.size() == 1)
            Error::showError(GLB_INIT_ERR);
        else  
        {
            // isInit = true;
            return true; //局部变量初始化，留给gen部分处理
        }
    }
    return false;
}

void Var::clear()
{
    scopePath.clear();
    scopePath.push_back(-1); //全局作用域
    type = KW_INT; //默认为int
    literal = false;
    isExtern = false;
    isArray = false; 
    arrayLen = 0; 
    isPtr = false;
    isLeft = true; //默认为左值
    initVal = NULL; 
    isInit = false; //默认初始化失败
    ptr = NULL;
    size = offset = 0;
}

Var::Var()
{
    clear();
    setName("<void>");
    isLeft = false;
    intVal = 0;
    type = KW_VOID;
    isPtr = true; //消除基本类型？ 
}

Var::~Var()
{}

//拷贝变量
Var::Var(vector<int> &scp, Var* var)
{
    clear();
    scopePath = scp;
    setType(var->getType());
    setPtr(var->isPtr || var->isArray); //设置指针
    isLeft = false; //不能作为左值
    setName("");
}

//临时变量
Var::Var(vector<int> &scp, Tag t, bool ptr)
{
    clear();
    scopePath = scp;
    setType(t);
    setPtr(ptr);
    setName("");
    isLeft = false; //默认不能作为左值
}

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
    isLeft = false; //常量不能作为左值
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
        intVal = charVal; //可以将char转换为int
    }
    else 
    {
        //字符串以字符数组存下来
        setType(KW_CHAR);
        strVal = ((Str*)it)->str;
        if(strVal == "%d") name = ".L0";
        else name = GenIR::genLb(); //创建一个名字 后面代码生成需要根据名字来确定字符串的其实地址
        setArray(strVal.size() + 1); //+1表示留一个结束符
    }
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
    curEsp = ARM::stackBase; //记录栈深
    maxDep = ARM::stackBase;
    for(int i = 0, off = 4; i < paraVar.size(); i ++, off += 4)
        paraVar[i]->setOffset(off);
    //TODO 计算参数相对于栈帧基址的偏移值
}

bool Fun::matchFun(Fun* fun)
{
    if(fun->getName() != name) return false;
    if(fun->getPara().size() != paraVar.size()) return false;
    vector<Var*> &list = fun->getPara();
    for(int i = 0; i < paraVar.size(); i ++)
        if(!GenIR::typeCheck(list[i], paraVar[i]))
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

void Fun::locate(Var* var)
{
    int size = var->getSize();
    size += (4 - size % 4) % 4; //4字节对齐
    scopeEsp[scopeEsp.size() - 1] += size;
    curEsp += size;
    var->setOffset(-curEsp); //设置偏移量
}

void Fun::addCode(Quaternion *code)
{
    incode.addQuaternion(code);
}

void Fun::toStringFun()
{
    cout << "Function:\n";
    cout << "Type: " + TagName[type] << endl;
    cout << "Name: " + name << endl;
    cout << "Paras: " << endl;
    for(auto i: paraVar) cout << i->getName() << " ";
    printf("\n------------------------\n");
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

void Fun::setReturnPoint(Quaternion *point)
{
    returnPoint = point;
}

Quaternion* Fun::getReturnPoint()
{
    return returnPoint;
}

void Fun::printInterCode(int isopt)
{
    int codeNum = 0;
    if(!isopt)
    {
        for(auto i: incode.intercode)
            printf("Code %d ", ++ codeNum), i->toString();
    }
    else
    {
        for(auto i: optincode)
            printf("Code %d ", ++ codeNum), i->toString();
    }
}

int Fun::getMaxDep()
{
    return maxDep;
}

void Fun::getObjCode(FILE* file, bool isopt)
{
    //TODO 中间代码优化
    vector<Quaternion*> intercode;
    if(!isopt) 
        intercode = incode.getInterCode(); //未优化代码
    else
    {
        for(auto code: optincode)
            intercode.push_back(code); //优化代码
    }
    fprintf(file, "\t.global %s\n", name.c_str());
    fprintf(file, "%s:\n", name.c_str());
    Obj obj;
    Translate translate(intercode, obj);
    translate.transArm();
    obj.printObjCode(file);
}

void Fun::optimize(SymTab* symtab)
{
    dfg = new DFG(incode); //创建数据流图
    ConstPropagation conpro(symtab, dfg, paraVar);
    conpro.propagate();
    dfg->getCode(optincode);
}