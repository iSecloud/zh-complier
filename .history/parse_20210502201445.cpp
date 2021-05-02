#include <vector>
#include <ctime>
#include "parse.h"
#include "lex.h"
#include "token.h"
#include "symbol.h"
#include "error.h"

Parser::Parser(Lex &lexer1): lexer(lexer1) 
{}

Parser::~Parser() 
{}

Tag Parser::GetTag()
{
    Tag t = lookahead->tag;
    move();
    return t;
}

void Parser::move()
{
    lookahead = lexer.getToken();
}

bool Parser::match(int need)
{
    if(lookahead->tag == need)
    {
        move();
        return true;
    }
    return false;
}

#define FOLLOW vector<int>
#define LALO_FIRST ASSIGN, OOR, AAND, OR, XOR, AND, GT, GE, LT, LE, EQU, NEQU, ADD, SUB, MUL, \
MOD, DIV, INC, DEC
#define RALO_FIRST OOR, AAND, OR, XOR, AND, GT, GE, LT, LE, EQU, NEQU, ADD, SUB, MUL, DIV, MOD
#define EXPR_FIRST INC, DEC, MUL, NNOT, SUB, NOT, AND, LPAREN, NUM, CHAR, STR, ID
#define TYPE_FIRST KW_INT, KW_CHAR, KW_VOID
#define STAT_FIRST KW_WHILE, KW_DO, KW_SWITCH, KW_IF, KW_FOR, KW_SECLOUD, KW_READ, \
KW_WRITE, KW_RETURN, KW_BREAK, KW_CONTINUE, SEMICON

//是否在follow集里面
bool Parser::isInFollow(vector<int> a)
{
    if(find(a.begin(), a.end(), lookahead->tag) != a.end()) 
        return true;
    else 
        return false;
}

void Parser::recovery(bool real, Err errCode)
{
    Error::showError(errCode);
    //不可以与follow集匹配
    if(!real) move(); 
}

void Parser::Program()
{
    Segment();
    Program();
}

void Parser::Segment()
{
    bool ext = match(KW_EXTERN);
    Tag t = Type();
    Def(t, ext);
}

Tag Parser::Type()
{
    Tag t = KW_INT;
    if(isInFollow(FOLLOW{TYPE_FIRST}))
        t = GetTag();
    else 
        recovery(isInFollow(FOLLOW{ID, MUL}), TYPE_LOST);
    return t;
}

Var* Parser::Init(Tag t, bool ext, string name, bool ptr)
{
    Var* val = NULL;
    if(match(ASSIGN)) val = expr();
    //TODO 这里需要新建一个变量并返回
}

void Parser::Def(Tag t, bool ext)
{
    bool mul = match(MUL);
    string idName = ""; //默认为空
    if(lookahead->tag == ID) 
    {
        idName = ((Id*)lookahead)->id;
        move();
        if(!mul) DefTail(t, ext, idName, 0);
        else
        {
            Init(t, ext, idName, 1); //TODO:添加一个指针变量
            VarList(t, ext);
        } 
    }
    else
    {
        if(mul)
        {
            recovery(isInFollow(FOLLOW{ASSIGN, SEMICON, COMMA}), IDNAME_LOST);
            Init(t, ext, idName, 1); //TODO:添加一个指针变量
            VarList(t, ext);
        }
        else
        {
            recovery(isInFollow(FOLLOW{ASSIGN, SEMICON, COMMA, LPAREN, LBRACKET}), IDNAME_LOST);
            DefTail(t, ext, idName, 0);
        }
    }
}

void Parser::DefTail(Tag t, bool ext, string name, bool ptr)
{
    if(match(LPAREN)) //函数
    {
        //TODO: 记录函数路径, 进入函数
        Tag t = Type();
        vector<Var*> paraList; //参数列表
        Para(paraList);
        if(!match(RPAREN)) recovery(isInFollow(FOLLOW{SEMICON, LBRACE}), RPAREN_LOST);
        Fun* Fun; //TODO: 新建一个函数
        FunTail(Fun);
        //TODO: 记录函数路径, 退出函数
    }
    else //普通变量定义
    {
        NorVarDef(t, ext, name, ptr); //TODO: 添加一个变量
        VarList(t, ext);
    }
}

Var* Parser::NorVarDef(Tag t, bool ext, string name, bool ptr)
{
    if(match(LBRACKET))
    {
        int num = 0;
        if(lookahead->tag == NUM)
        {
            num = ((Num*)lookahead)->num;
            move();
        }
        else recovery(isInFollow(FOLLOW{RBRACKET}), NUM_LOST);

        if(!match(RBRACKET)) recovery(isInFollow(FOLLOW{COMMA, SEMICON}), RBRACKET_LOST);

        //TODO: 这里需要返回一个数组变量
    }
    else
    {
        return Init(t, ext, name, ptr);
    }
}

void Parser::VarList(Tag t, bool ext)
{
    if(match(SEMICON)) return;

    if(match(COMMA))
    {
        VarDef(t, ext); // TODO: 这里需要添加一个变量
        VarList(t, ext);
    }
    else
    {
        if(isInFollow(FOLLOW{ID, MUL}))
        {
            recovery(true, COMMA_LOST);
            VarDef(t, ext); // TODO: 这里需要添加一个变量
            VarList(t, ext);
        }
        else
        {
            recovery(isInFollow(FOLLOW{KW_EXTERN, RBRACE, KW_CASE, KW_DEFAULT, TYPE_FIRST, STAT_FIRST, EXPR_FIRST}), SEMICON_LOST);
        }
    }
}

Var* Parser::VarDef(Tag t, bool ext)
{
    bool mul = match(MUL);
    string idName = "";
    if(lookahead->tag == ID)
    {
        idName = ((Id*)lookahead)->id;
        move();
        if(!mul) return NorVarDef(t, ext, idName, mul);
        else return Init(t, ext, idName, mul);
    }
    else
    {
        if(!mul)
        {
            recovery(isInFollow(FOLLOW{LBRACKET, ASSIGN, COMMA, SEMICON}), IDNAME_LOST);
            return NorVarDef(t, ext, idName, mul);
        }
        else
        {
            recovery(isInFollow(FOLLOW{ASSIGN, COMMA, SEMICON}), IDNAME_LOST);
            return Init(t, ext, idName, mul);
        }
    }
}

void Parser::Para(vector<Var*> &paraList)
{
    if(lookahead->tag == RPAREN) return;
    Tag t = Type();
    Var* pr = ParaDef(t);
    // TODO 这里需要添加参数变量
    paraList.push_back(pr);
    ParaList(paraList);

}

Var* Parser::ParaDef(Tag t)
{
    bool mul = match(MUL);
    string idName = "";
    if(lookahead->tag == ID)
    {
        idName = ((Id*)lookahead)->id;
        move();
        if(mul)
        {
            //TODO 这里需要返回一个指针变量
        }
        else
        {
            return ParaTail(t, idName);
        }
    }
    else
    {
        if(mul)
        {
            recovery(isInFollow(FOLLOW{COMMA, RPAREN}), IDNAME_LOST);
            //TODO 这里需要返回一个指针变量
        }
        else
        {
            recovery(isInFollow(FOLLOW{COMMA, RPAREN, LBRACKET}), IDNAME_LOST);
            return ParaTail(t, idName);
        }
    }
}

Var* Parser::ParaTail(Tag t, string name)
{
    if(!match(LBRACKET))
    {
        //TODO 这里需要新建一个变量并返回
    }
    else
    {
        int num = 0;
        if(lookahead->tag == NUM)
        {
            num = ((Num*)lookahead)->num;
            move();
        }//函数传数组可以不指定长度

        if(!match(RBRACKET)) 
            recovery(isInFollow(FOLLOW{COMMA, RPAREN}), RPAREN_LOST);
        //TODO 这里需要新建一个变量并返回
    }
}

void Parser::ParaList(vector<Var*> &paraList)
{
    if(!match(COMMA)) return;
    Tag t = Type();
    Var* pr = ParaDef(t);
    // TODO 这里需要添加参数变量
    paraList.push_back(pr);
    ParaList(paraList);
}

void Parser::FunDef()
{}

void Parser::FunTail(Fun* Fun)
{
    if(match(SEMICON))
    {
        //TODO 添加一个声明函数
    }
    else
    {
        //TODO 添加一个定义函数
        FunBody();
        //TODO 结束函数定义
    }
}

void Parser::FunBody()
{
    if(!match(LBRACE)) 
        recovery(isInFollow(FOLLOW{TYPE_FIRST, STAT_FIRST, RBRACE}), LBRACE_LOST);
    FunProgram();
    if(!match(RBRACE))
        recovery(isInFollow(FOLLOW{KW_EXTERN, TYPE_FIRST, STAT_FIRST, EXPR_FIRST}), RBRACE_LOST);
}

void Parser::FunProgram()
{
    if(isInFollow(FOLLOW{TYPE_FIRST})) 
        LocalDef();
    else if(isInFollow(FOLLOW{STAT_FIRST, EXPR_FIRST}))
        Statement();
    FunProgram();
}

void Parser::LocalDef()
{
    Tag t = Type();
    VarDef(t, false); //TODO 这里需要添加变量
    VarList(t, false);
}

void Parser::Statement()
{
    Tag t = lookahead->tag;
    if(t == KW_WHILE) WhileStat();
    else if(t == KW_DO) DoWhileStat();
    else if(t == KW_FOR) ForStat();
    else if(t == KW_IF) IfStat();
    else if(t == KW_SWITCH) SwitchStat();
    else if(t == KW_SECLOUD) SecloudStat();
    else if(t == KW_READ) ReadStat();
    else if(t == KW_WRITE) WriteStat(0);
    else if(t == KW_BREAK) 
    {
        //TODO 产生break语句
        move();
        if(!match(SEMICON))
            recovery(isInFollow(FOLLOW{KW_EXTERN, RBRACE, KW_CASE, KW_DEFAULT, TYPE_FIRST, STAT_FIRST, EXPR_FIRST}), SEMICON_LOST);
    }
    else if(t == KW_CONTINUE)
    {
        //TODO 产生continue语句
        if(!match(SEMICON))
            recovery(isInFollow(FOLLOW{KW_EXTERN, RBRACE, KW_CASE, KW_DEFAULT, TYPE_FIRST, STAT_FIRST, EXPR_FIRST}), SEMICON_LOST);
    }
    else if(t == KW_RETURN)
    {
        move();
        //TODO 产生return语句，注意可能有表达式
        if(!match(SEMICON))
            recovery(isInFollow(FOLLOW{KW_EXTERN, RBRACE, KW_CASE, KW_DEFAULT, TYPE_FIRST, STAT_FIRST, EXPR_FIRST}), SEMICON_LOST);
    }
    else
    {
        altexpr();
        if(match(SEMICON))
            recovery(isInFollow(FOLLOW{KW_EXTERN, RBRACE, KW_CASE, KW_DEFAULT, TYPE_FIRST, STAT_FIRST, EXPR_FIRST}), SEMICON_LOST);
    }
}

void Parser::WhileStat()
{
    //TODO 进入作用域
    move();
    if(!match(LPAREN))
        recovery(isInFollow(FOLLOW{TYPE_FIRST, RPAREN}), LPAREN_LOST);
    altexpr();
    //TODO 得到判断条件
    if(!match(RPAREN))
        recovery(isInFollow(FOLLOW{LBRACE}), RPAREN_LOST);
    FunBody();
    //TODO 离开作用域
} 

void Parser::DoWhileStat()
{
    move();
    //TODO 进入作用域
    FunBody();
    //TODO 离开作用域
    if(!match(KW_WHILE))
        recovery(isInFollow(FOLLOW{LPAREN}), WHILE_LOST);
    if(!match(LPAREN))
        recovery(isInFollow(FOLLOW{TYPE_FIRST, RPAREN}), LPAREN_LOST);
    altexpr();
    //TODO 得到判断条件
    if(!match(RPAREN))
        recovery(isInFollow(FOLLOW{SEMICON}), RPAREN_LOST);
    if(!match(SEMICON))
        recovery(isInFollow(FOLLOW{KW_EXTERN, RBRACE, KW_CASE, KW_DEFAULT, TYPE_FIRST, STAT_FIRST, EXPR_FIRST}), SEMICON_LOST);
}

void Parser::ForStat()
{
    move();
    //TODO 进入作用域 
    if(!match(LPAREN))
        recovery(isInFollow(FOLLOW{TYPE_FIRST, EXPR_FIRST, SEMICON}), LPAREN_LOST);
    ForInit();
    if(!match(SEMICON))
        recovery(isInFollow(FOLLOW{EXPR_FIRST}), SEMICON_LOST);
    altexpr();
    //TODO 得到判断条件
    if(!match(SEMICON))
        recovery(isInFollow(FOLLOW{EXPR_FIRST}), SEMICON_LOST);
    altexpr();
    if(!match(RPAREN))
        recovery(isInFollow(FOLLOW{LBRACE}), RPAREN_LOST);
    FunBody();
    //TODO 退出作用域
}

void Parser::ForInit()
{
    if(isInFollow(FOLLOW{TYPE_FIRST}))
        LocalDef();
    else 
        altexpr();
}

void Parser::IfStat()
{
    move();
    if(!match(LPAREN))
        recovery(isInFollow(FOLLOW{EXPR_FIRST, RPAREN}), LPAREN_LOST);
    altexpr();
    //TODO 得到判断条件
    if(!match(RPAREN))
        recovery(isInFollow(FOLLOW{LBRACE}), RPAREN_LOST);
    //TODO 进入作用域
    FunBody();
    //TODO 退出作用域
    ElseStat();
}

void Parser::ElseStat()
{
    if(match(KW_ELSE))
    {
        //TODO 进入作用域
        FunBody();
        //TODO 退出作用域
    }
}

void Parser::SwitchStat()
{
    move();
    //TODO 进入作用域
    if(!match(LPAREN))
        recovery(isInFollow(FOLLOW{EXPR_FIRST}), LPAREN_LOST); //注意这里必须有表达式？
    expr();
    //TODO 得到表达式的值
    if(!match(RPAREN))
        recovery(isInFollow(FOLLOW{LBRACE}), RPAREN_LOST);
    if(!match(LBRACE))
        recovery(isInFollow(FOLLOW{KW_CASE, KW_DEFAULT}), LBRACE_LOST);
    CaseStat();
    if(!match(RBRACE))
        recovery(isInFollow(FOLLOW{KW_EXTERN, TYPE_FIRST, STAT_FIRST, EXPR_FIRST}), RBRACE_LOST);
    //TODO 离开作用域
}

void Parser::CaseStat()
{
    if(match(KW_CASE))
    {
        literal(); //TODO 得到常量表达式
        if(!match(COLON))
            recovery(isInFollow(FOLLOW{TYPE_FIRST, STAT_FIRST}), COLON_LOST);
        //TODO 进入作用域
        FunProgram();
        //TODO 退出作用域
        CaseStat();
    }
    else if(match(KW_DEFAULT))
    {
        if(!match(COLON))
            recovery(isInFollow(FOLLOW{TYPE_FIRST, STAT_FIRST}), COLON_LOST);
        //TODO 进入作用域
        FunProgram();
        //TODO 退出作用域
    }
}

void Parser::SecloudStat()
{
    move();
    if(!match(LPAREN))
        recovery(isInFollow(FOLLOW{NUM}), LPAREN_LOST);
    int num = 0;
    if(!match(NUM))
        recovery(isInFollow(FOLLOW(RPAREN)), NUM_LOST);
    else 
        WriteStat(RadNum());
    if(!match(RPAREN))
        recovery(isInFollow(FOLLOW{SEMICON}), RPAREN_LOST);
    if(!match(SEMICON))
        recovery(isInFollow(FOLLOW{KW_EXTERN, RBRACE, KW_CASE, KW_DEFAULT, TYPE_FIRST, STAT_FIRST, EXPR_FIRST}), SEMICON_LOST);
}

int Parser::RadNum()
{
    srand(time(NULL));
    int num[] = {123, 57, 152, 110};
    return num[rand() % 4];
}

void Parser::ReadStat()
{}

void Parser::WriteStat(int num)
{}

Var* Parser::altexpr()
{
    if(isInFollow(FOLLOW{EXPR_FIRST}))
        expr();
    else return NULL; //TODO 返回一个空变量
}

Var* Parser::expr()
{
    return assexpr();
}

Var* Parser::assexpr()
{
    Var* leftVal = oorexpr();
    return asstail(leftVal);
}

Var* Parser::asstail(Var* leftVal)
{
    if(!match(ASSIGN)) return leftVal;
    Var* rightVal = assexpr();
    Var* ans; //TODO 记录左值与右值的运算答案
    return ans; 
    //return asstial(ans) ?
}

Var* Parser::oorexpr()
{
    Var* leftVal = aandexpr();
    return oortail(leftVal);
}

Var* Parser::oortail(Var* leftVal)
{
    if(!match(OOR)) return leftVal;
    Var* rightVal = aandexpr();
    Var* ans; //TODO 记录左值与右值的运算答案
    return oortail(ans);
}

Var* Parser::aandexpr()
{
    Var* leftVal = orexpr();
    return aandtail(leftVal);
}

Var* Parser::aandtail(Var* leftVal)
{
    if(!match(AAND)) return leftVal;
    Var* rightVal = orexpr();
    Var* ans; //TODO 记录左值与右值的运算答案
    return aandtail(ans);
}

Var* Parser::orexpr()
{
    Var* leftVal = xorexpr();
    return ortail(leftVal);
}

Var* Parser::ortail(Var* leftVal)
{
    if(!match(OR)) return leftVal;
    Var* rightVal = xorexpr();
    Var* ans; //TODO 记录左值与右值的运算答案
    return ortail(ans);
}

Var* Parser::xorexpr()
{
    Var* leftVal = andexpr();
    return xortail(leftVal);
}

Var* Parser::xortail(Var* leftVal)
{
    if(!match(XOR)) return leftVal;
    Var* rightVal = andexpr();
    Var* ans; //TODO 记录左值与右值的运算答案
    return xortail(ans);
}

Var* Parser::andexpr()
{
    Var* leftVal = cmpexpr();
    return andtail(leftVal);
}

Var* Parser::andtail(Var* leftVal)
{
    if(!match(AND)) return leftVal;
    Var* rightVal = cmpexpr();
    Var* ans; //TODO 记录左值与右值的运算答案
    return andtail(ans);
}

Var* Parser::cmpexpr()
{
    Var* leftVal = aloexpr();
    return cmptail(leftVal);
}

Var* Parser::cmptail(Var* leftVal)
{
    if(!isInFollow(FOLLOW{GT, GE, LT, LE, NEQU, EQU})) 
        return leftVal;
    Tag cp = cmps();
    Var* rightVal = aloexpr();
    Var* ans; //TODO 记录左值与右值的运算答案
    return cmptail(ans);
}

Tag Parser::cmps()
{
    return GetTag();
}

Var* Parser::aloexpr()
{
    Var* leftVal = item();
    return alotail(leftVal);
}

Var* Parser::alotail(Var* leftVal)
{
    if(!isInFollow(FOLLOW{ADD, SUB})) return leftVal;
    Tag al = alos();
    Var* rightVal = item();
    Var* ans; //TODO 记录左值与右值的运算答案
    return alotail(ans);
}

Tag Parser::alos()
{
    return GetTag();
}

Var* Parser::item()
{
    Var* leftVal = factor();
    return itemtail(leftVal);
}

Var* Parser::itemtail(Var* leftVal)
{
    if(!isInFollow(FOLLOW{MUL, DIV, MOD})) return leftVal;
    Tag it = its();
    Var* rightVal = factor();
    Var* ans; //TODO 记录左值与右值的运算答案
    return itemtail(ans);
}

Tag Parser::its()
{
    return GetTag();
}

Var* Parser::factor()
{
    if(isInFollow(FOLLOW{NNOT, NOT, AND, MUL, SUB, INC, DEC}))
    {
        Tag lp = lop();
        Var* ans = factor();
        //TODO 记录单目运算的运算答案
        return ans;
    }
    else return val();
}

Tag Parser::lop()
{
    return GetTag();
}

Var* Parser::val()
{
    Var* ans = element();
    if(isInFollow(FOLLOW{INC, DEC}))
    {
        Tag rp = rop();
        //TODO 记录单目运算的运算答案
    }
    return ans;
}

Tag Parser::rop()
{
    return GetTag();
}

Var* Parser::element()
{
    if(match(ID))
    {
        string name = ((Id*)lookahead)->id;
        move();
        return idexpr(name);
    }
    else if(match(LPAREN))
    {
        Var* exp = expr();
        if(!match(RPAREN))
            recovery(isInFollow(FOLLOW{LALO_FIRST}), RPAREN_LOST);
        return exp;
    }
    else return literal();
}

Var* Parser::idexpr(string name)
{
    if(match(LBRACKET))
    {
        Var* len = expr();
        if(!match(RBRACKET))
            recovery(isInFollow(FOLLOW{LALO_FIRST}), RBRACKET_LOST);
        Var* array;
        //TODO 获取数组
        return array;
    }
    else if(match(LPAREN))
    {
        vector<Var*> argList;
        realarg(argList);
        if(!match(RPAREN))
            recovery(isInFollow(FOLLOW{RALO_FIRST}), RPAREN_LOST);
        Fun* fun;
        //TODO 获取函数并进行运算
        Var* ans;
        return ans;
    }
    else
    {
        Var* ans;
        //TODO 获取变量
        return ans;
    }
}